#include <iostream>
#include <antlr4-runtime.h>
#include "parser-specs/configLexer.h"
#include "parser-specs/configParser.h"
#include "parser-specs/configBaseListener.h"
#include "base_config_applier.h"
#include "criteria_state.h"
#include "config_parser.h"
#include "fn.hpp"
#include "config_applier.h"
#include <ranges>
#include <utility>
#include <algorithm>
#include <xcb/xcb_xrm.h>

using namespace std;
using namespace antlr4;
using namespace std::literals;

namespace fn = rangeless::fn;
using fn::operators::operator%;   // arg % fn   equivalent to fn(std::forward<Arg>(arg))
using fn::operators::operator%=;  // arg %= fn; equivalent to arg = fn( std::move(arg));

class ResourceDatabase {
    xcb_xrm_database_t *database = nullptr;
    xcb_connection_t *conn;
   public:

    explicit ResourceDatabase(xcb_connection_t *conn) : conn(conn) {
    }

    std::string get_resource(std::string &name, std::string &fallback) {
        /* Load the resource database lazily. */
        if (database == nullptr) {
            database = xcb_xrm_database_from_default(conn);

            if (database == nullptr) {
                ELOG("Failed to open the resource database.\n");

                /* Load an empty database so we don't keep trying to load the
                 * default database over and over again. */
                database = xcb_xrm_database_from_string("");

                return fallback;
            }
        }

        char *resource;
        xcb_xrm_resource_get_string(database, name.c_str(), nullptr, &resource);

        if (resource == nullptr) {
            DLOG(fmt::sprintf("Could not get resource '%s', using fallback '%s'.\n",  name, fallback));
            return fallback;
        }

        return resource;
    }

    ~ResourceDatabase() {
        if (database) {
            xcb_xrm_database_free(database);
        }
    }
};


class VariableListener : public configBaseListener {
private:
    ResourceDatabase resourceDatabase;
public:
    std::map<std::string, std::string> variables{};

    explicit VariableListener(xcb_connection_t *conn) : resourceDatabase{conn} {
    }

    void enterSet(configParser::SetContext *ctx) override {
        auto variable = ctx->VARIABLE()->getText();
        auto value = ctx->arguments()->children
                     % fn::transform([](auto *x) { return x->getText(); })
                     % fn::join(" ");

        variables[variable] = value;
    }

    void enterSet_from_resource(configParser::Set_from_resourceContext *ctx) override {
        auto variable = ctx->VARIABLE()->getText();
        auto key = ctx->STRING(0)->getText();
        auto fallback = ctx->STRING(1) != nullptr ? ctx->STRING(1)->getText() : "";
        std::string res_value = resourceDatabase.get_resource(key, fallback);
        variables[variable] = res_value;
    }

};

class StatementListener : public configBaseListener {

private:
    BaseConfigApplier &applier;
    std::map<std::string, std::string> variables;
    bool inMode = false;

    static auto optional_str(tree::TerminalNode *node) {
        if (node != nullptr) {
            static string value = node->getText();
            return std::optional<std::reference_wrapper<std::string>>{value};
        } else {
            return std::optional<std::reference_wrapper<std::string>>{};
        }
    }

    std::string replace_var(std::string &&input) {
        std::string replaced = input;
        for (auto &v : variables) {
            for (auto i = replaced.find(v.first); i != string::npos; i = replaced.find(v.first)) {
                replaced.replace(i, v.first.size(), v.second);
            }
        }

        return replaced;
    }

    Match handle_criteria(configParser::CriteriaContext *pContext) {
        Match current_match{};

        for (auto &c : pContext->criterion()) {
            if (c->value_criterion() != nullptr) {
                current_match.parse_property(
                        c->value_criterion()->children[0]->getText().c_str(),
                        c->value_criterion()->children[1]->getText().c_str());
            } else {
                current_match.parse_property(c->children[0]->getText().c_str(), nullptr);
            }
        }

        return current_match;
    }

public:
    StatementListener(BaseConfigApplier &listener, std::map<std::string, std::string> &&variables) : applier{listener}, variables{variables} {
        for (auto &v : variables) {
            std::cout << "set " << v.first << " = " << v.second << std::endl;
        }
    }

    void enterColor_single(configParser::Color_singleContext *ctx) override {
        auto colorclass = replace_var(ctx->children[0]->getText());
        auto color = replace_var(ctx->STRING()->getText());
        applier.color_single(colorclass, color);
    }

    void enterColor_multiple(configParser::Color_multipleContext *ctx) override {
        auto colorclass = replace_var(ctx->children[0]->getText());
        auto border = replace_var(ctx->STRING(0)->getText());
        auto background = replace_var(ctx->STRING(1)->getText());
        auto text = replace_var(ctx->STRING(2)->getText());
        auto indicator = optional_str(ctx->STRING(3)).transform([](auto f) { return f.get(); }).value_or(""s);
        auto child_border = optional_str(ctx->STRING(4)).transform([](auto f) { return f.get(); }).value_or(""s);
        applier.color(colorclass, border, background, text, indicator, child_border);
    }

    void enterExec(configParser::ExecContext *ctx) override {
        const string text = replace_var(ctx->children[0]->getText());
        auto no_startup_id = ctx->exec_options()
                       % fn::exists_where([](auto *x) { return x->getText() == "--no-startup-id"; });

        auto arguments = replace_var(ctx->arguments()->children
                         % fn::transform([](auto *x) { return x->getText(); })
                         % fn::join(" "));

        applier.exec(text, no_startup_id, arguments);
    }

    void enterPopup_during_fullscreen(configParser::Popup_during_fullscreenContext *ctx) override {
        const string value = replace_var(ctx->popup_during_fullscreen_value()->getText());
        applier.popup_during_fullscreen(value);
    }

    void enterRestart_state(configParser::Restart_stateContext *ctx) override {
        auto path = replace_var(ctx->STRING()->getText());
        applier.restart_state(path);
    }

    void enterIpc_kill_timeout(configParser::Ipc_kill_timeoutContext *ctx) override {
        auto timeout = replace_var(ctx->NUMBER()->getText());
        applier.ipc_kill_timeout(std::stol(timeout));
    }

    void enterIpc_socket(configParser::Ipc_socketContext *ctx) override {
        auto path = replace_var(ctx->STRING()->getText());
        applier.ipc_socket(path);
    }

    void enterWorkspace(configParser::WorkspaceContext *ctx) override {
        auto workspace = replace_var(ctx->workspace_name()->getText());
        auto output = replace_var(ctx->STRING(0)->getText());
        applier.workspace(workspace, output);
    }

    void enterTitle_align(configParser::Title_alignContext *ctx) override {
        auto align = replace_var(ctx->title_align_value()->getText());
        applier.title_align(align);
    }

    void enterFocus_on_window_activation(configParser::Focus_on_window_activationContext *ctx) override {
        auto value = replace_var(ctx->focus_on_window_activation_value()->getText());
        applier.focus_on_window_activation(value);
    }

    void enterForce_display_urgency_hint(configParser::Force_display_urgency_hintContext *ctx) override {
        auto value = replace_var(ctx->NUMBER()->getText());
        applier.force_display_urgency_hint(std::stol(value));
    }

    void enterWorkspace_auto_back_and_forth(configParser::Workspace_auto_back_and_forthContext *ctx) override {
        auto value = replace_var(ctx->BOOL()->getText());
        applier.workspace_back_and_forth(value);
    }

    void enterFocus_wrapping(configParser::Focus_wrappingContext *ctx) override {
        auto value = replace_var(ctx->BOOL()->getText());
        applier.focus_wrapping(value);
    }

    void enterForce_focus_wrapping(configParser::Force_focus_wrappingContext *ctx) override {
        auto value = replace_var(ctx->BOOL()->getText());
        applier.force_focus_wrapping(value);
    }

    void enterMouse_warping(configParser::Mouse_warpingContext *ctx) override {
        auto value = replace_var(ctx->mouse_warping_value()->getText());
        applier.mouse_warping(value);
    }

    void enterFocus_follows_mouse(configParser::Focus_follows_mouseContext *ctx) override {
        auto value = replace_var(ctx->BOOL()->getText());
        applier.focus_follows_mouse(value);
    }

    void enterNo_focus(configParser::No_focusContext *ctx) override {
        Match match = handle_criteria(ctx->criteria());
        applier.no_focus(match);
    }

    void enterAssign(configParser::AssignContext *ctx) override {
        Match match = handle_criteria(ctx->criteria());
        if (ctx->assign_target()->assign_target_output() != nullptr) {
            string value = replace_var(ctx->assign_target()->assign_target_output()->children[1]->getText());
            applier.assign_output(match, value);
        } else if (ctx->assign_target()->assign_target_workspace() != nullptr) {
            string value = replace_var(ctx->assign_target()->assign_target_workspace()->children[1]->getText());
            applier.assign(match, value, false);
        } else if (ctx->assign_target()->assign_target_number() != nullptr) {
            string value = replace_var(ctx->assign_target()->assign_target_number()->children[1]->getText());
            applier.assign(match, value, true);
        }
    }

    void enterFor_window(configParser::For_windowContext *ctx) override {
        auto arguments = replace_var(ctx->arguments()->children
                         % fn::transform([](auto *x) { return x->getText(); })
                         % fn::join(" "));

        Match match = handle_criteria(ctx->criteria());
        applier.for_window(match, arguments);
    }

    void enterHide_edge_borders(configParser::Hide_edge_bordersContext *ctx) override {
        auto border_type = replace_var(ctx->border_type()->getText());
        applier.hide_edge_borders(border_type);
    }

    void enterDefault_border(configParser::Default_borderContext *ctx) override {
        auto defaultBorderType = replace_var(ctx->default_border_type()->getText());
        auto width = replace_var(ctx->border_style()->NUMBER()->getText()); // TODO check for null?
        auto border = replace_var(ctx->border_style()->children[0]->getText());
        applier.default_border(defaultBorderType, border, std::stol(width));
    }

    void enterWorkspace_layout(configParser::Workspace_layoutContext *ctx) override {
        auto value = replace_var(ctx->workspace_layout_value()->getText());
        applier.workspace_layout(value);
    }

    void enterDefault_orientation(configParser::Default_orientationContext *ctx) override {
        auto value = replace_var(ctx->default_orientation_value()->getText());
        applier.default_orientation(value);
    }

    void enterFloating_modifier(configParser::Floating_modifierContext *ctx) override {
        auto arguments = replace_var(ctx->modifier()
                         % fn::transform([](auto *x) { return x->getText(); })
                         % fn::join(" "));

        applier.floating_modifier(arguments);
    }

    void enterFloating_minimum_size(configParser::Floating_minimum_sizeContext *ctx) override {
        auto w = replace_var(ctx->dimension()->NUMBER(0)->getText());
        auto h = replace_var(ctx->dimension()->NUMBER(1)->getText());
        applier.floating_minimum_size(std::stol(w), std::stol(h));
    }

    void enterFloating_maximum_size(configParser::Floating_maximum_sizeContext *ctx) override {
        auto w = replace_var(ctx->dimension()->NUMBER(0)->getText());
        auto h = replace_var(ctx->dimension()->NUMBER(1)->getText());
        applier.floating_maximum_size(std::stol(w), std::stol(h));
    }

    void enterFont(configParser::FontContext *ctx) override {
        auto arguments = replace_var(ctx->arguments()->children
                % fn::transform([](auto *x) { return x->getText(); })
                % fn::join(" "));
        applier.font(arguments);
    }

    void enterInclude(configParser::IncludeContext *ctx) override { }
    void exitInclude(configParser::IncludeContext *ctx) override { }

    void enterBinding(configParser::BindingContext *ctx) override {
        auto bindtype = replace_var(ctx->children[0]->getText());

        auto options = ctx->binding_options()
                % fn::transform([](auto opt) { return opt->getText(); })
                % fn::unique_all()
                % fn::to_vector();

        auto command = replace_var(ctx->arguments()->children
                       % fn::transform([](auto opt) { return opt->getText(); })
                       % fn::join(" "));

        auto release = options
                % fn::exists_where([](auto text) { return text == "--release"; });

        auto border = options % fn::exists_where([](auto text) { return text == "--border"; });

        auto whole_window = options % fn::exists_where([](auto text) { return text == "--whole-window"; });

        auto exclude_titlebar = options % fn::exists_where([](auto text) { return text == "--exclude-titlebar"; });

        std::string modifiers;
        std::string key;

        std::string keybinding = replace_var(ctx->keybinding()->getText());

        auto pos = keybinding.find_last_of('+');

        if (pos != string::npos) {
            modifiers = keybinding.substr(0, pos);
            key = keybinding.substr(pos + 1);
        } else {
            key = keybinding;
        }

        if (inMode) {
            applier.mode_binding(bindtype, modifiers, key, release, border, whole_window, exclude_titlebar, command);
        } else {
            applier.binding(bindtype, modifiers, key, release, border, whole_window, exclude_titlebar, command);
        }
    }


    void enterStmt_mode(configParser::Stmt_modeContext *ctx) override {
        auto pango_markup = ctx->stmt_mode_options() % fn::exists_where([](auto text) { return text->getText() == "--pango_markup"; });

        auto mode = replace_var(ctx->STRING()->getText());
        applier.enter_mode(pango_markup, mode);

        inMode = true;
    }

    void exitStmt_mode(configParser::Stmt_modeContext *ctx) override {
        inMode = false;
    }

    void enterEveryRule(antlr4::ParserRuleContext *ctx) override {}

    void visitTerminal(antlr4::tree::TerminalNode *node) override {}

    void visitErrorNode(antlr4::tree::ErrorNode *node) override {
        cout << "ERROR" << endl;
        throw std::runtime_error("error");
    }

};

NewParser::NewParser(const char *filename, config_load_t load_type, BaseConfigApplier &applier) : applier(applier), filename(filename), load_type(load_type) {

}

class ErrorListener : public BaseErrorListener {
    void syntaxError(Recognizer *recognizer, Token * offendingSymbol, size_t line, size_t charPositionInLine,
                             const std::string &msg, std::exception_ptr e) override {
        std::cout << "ERROR" << std::endl;
    }

};

parse_file_result_t NewParser::parse_file() {

    istream *stream;
    ifstream ifstream{};

    if (filename == "<stdin>"s) {
        stream = &cin;
    } else {
        ifstream.open(filename);
        stream = &ifstream;
    }

    ANTLRInputStream input{*stream};
    configLexer lexer{&input};

    lexer.removeErrorListeners();
    lexer.addErrorListener(new ErrorListener());

    CommonTokenStream tokens{&lexer};
    configParser parser{&tokens};
    auto tree = parser.file();

    //parser.setErrorHandler(new BailErrorStrategy());
    parser.removeErrorListeners();
    parser.addErrorListener(new ErrorListener());

    if (parser.getNumberOfSyntaxErrors() > 0) {
        throw std::runtime_error("we got a problem, sir");
    }

    //cout << tree->toStringTree(&parser, true) << endl << endl;

    // 1st pass, store all variables
    VariableListener variableListener{*global.a};
    tree::ParseTreeWalker::DEFAULT.walk(&variableListener, tree);

    // 2nd pass, execute instructions

    StatementListener listener{applier, std::move(variableListener.variables)};
    tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

    return parse_file_result_t::PARSE_FILE_SUCCESS;
}
