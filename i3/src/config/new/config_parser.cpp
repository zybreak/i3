#include <iostream>
#include <antlr4-runtime.h>
#include "parser-specs/configLexer.h"
#include "parser-specs/configGrammar.h"
#include "parser-specs/configGrammarBaseListener.h"
#include "../base_config_applier.h"
#include "config_parser.h"
#include "lib/fn.hpp"
#include "../resource_database.h"
#include <ranges>
#include <utility>
#include <algorithm>
#include <xcb/xcb_xrm.h>
#include <regex>

struct criteria_state;

using namespace std;
using namespace antlr4;
using namespace std::literals;

namespace fn = rangeless::fn;
using fn::operators::operator%;   // arg % fn   equivalent to fn(std::forward<Arg>(arg))
using fn::operators::operator%=;  // arg %= fn; equivalent to arg = fn( std::move(arg));

static string unquote(std::string &&quoted) {
    if (quoted.starts_with("\"") && quoted.ends_with("\"")) {
        return quoted.substr(1, quoted.length()-2);
    } else {
        return quoted;
    }
}

static string toString(antlr4::tree::ParseTree *p) {
    std::string str{};

    if (p->children.empty()) {
        str.append(p->getText());
        str.append(" ");
    } else {
        for (auto *c : p->children) {
            if (dynamic_cast<configGrammar::Command_execContext*>(p) != nullptr) {
                str.append(toString(c));
            } else {
                str.append(unquote(toString(c)));
            }
            str.append(" ");
        }
    }

    return str.substr(0, std::max((int)str.size() - 1, 0));
}

class VariableListener : public configGrammarBaseListener {
private:
    ResourceDatabase resourceDatabase;
public:
    std::map<std::string, std::string> variables{};

    explicit VariableListener(const ResourceDatabase &resourceDatabase) : resourceDatabase(resourceDatabase) {
    }

    void enterSet(configGrammar::SetContext *ctx) override {
        auto variable = ctx->VAR()->getText();
        const string quoted = unquote(ctx->STRING()->getText());

        variables[variable] = quoted;
    }

};

class StatementListener : public configGrammarBaseListener {

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

    criteria_state* handle_criteria(configGrammar::CriteriaContext *pContext) {
        criteria_state* st = applier.criteria_init(0);

        for (auto &c : pContext->criterion()) {
            if (c->value_criterion() != nullptr) {
                applier.criteria_add(st,
                        c->value_criterion()->children[0]->getText().c_str(),
                        c->value_criterion()->children[1]->getText().c_str());
            } else {
                applier.criteria_add(st, c->children[0]->getText().c_str(), nullptr);
            }
        }

        return st;
    }

public:
    StatementListener(BaseConfigApplier &listener, std::map<std::string, std::string> &&variables) : applier{listener}, variables{variables} {
        for (auto &v : variables) {
            std::cout << "set " << v.first << " = \"" << v.second << "\"" << std::endl;
        }
    }

    void enterExec(configGrammar::ExecContext *ctx) override {
        const string text = replace_var(ctx->children[0]->getText());
        auto options = ctx->OPTION();
        auto no_startup_id = options
                       % fn::exists_where([](auto *x) { return x->getText() == "--no-startup-id"; });

        auto arguments = replace_var(unquote(ctx->STRING()->getText()));

        applier.exec(text, no_startup_id, arguments);
    }

    void enterPopup_during_fullscreen(configGrammar::Popup_during_fullscreenContext *ctx) override {
        const string value = replace_var(unquote(ctx->STRING()->getText()));
        applier.popup_during_fullscreen(value);
    }

    void enterRestart_state(configGrammar::Restart_stateContext *ctx) override {
        auto path = replace_var(unquote(ctx->STRING()->getText()));
        applier.restart_state(path);
    }

    void enterIpc_kill_timeout(configGrammar::Ipc_kill_timeoutContext *ctx) override {
        auto timeout = replace_var(ctx->NUMBER()->getText());
        applier.ipc_kill_timeout(std::stol(timeout));
    }

    void enterIpc_socket(configGrammar::Ipc_socketContext *ctx) override {
        auto path = replace_var(unquote(ctx->STRING()->getText()));
        applier.ipc_socket(path);
    }

    void enterWorkspace(configGrammar::WorkspaceContext *ctx) override {
        auto workspace = replace_var(unquote(ctx->STRING(0)->getText()));
        auto output = replace_var(unquote(ctx->STRING(1)->getText())); // TODO out of bounds
        applier.workspace(workspace, output);
    }

    void enterTitle_align(configGrammar::Title_alignContext *ctx) override {
        auto align = replace_var(unquote(ctx->STRING()->getText()));
        applier.title_align(align);
    }

    void enterFocus_on_window_activation(configGrammar::Focus_on_window_activationContext *ctx) override {
        auto value = replace_var(unquote(ctx->STRING()->getText()));
        applier.focus_on_window_activation(value);
    }

    void enterForce_display_urgency_hint(configGrammar::Force_display_urgency_hintContext *ctx) override {
        auto value = replace_var(ctx->NUMBER()->getText());
        applier.force_display_urgency_hint(std::stol(value));
    }

    void enterWorkspace_auto_back_and_forth(configGrammar::Workspace_auto_back_and_forthContext *ctx) override {
        auto value = replace_var(unquote(ctx->STRING()->getText()));
        applier.workspace_back_and_forth(value);
    }

    void enterFocus_wrapping(configGrammar::Focus_wrappingContext *ctx) override {
        auto value = replace_var(unquote(ctx->STRING()->getText()));
        applier.focus_wrapping(value);
    }

    void enterForce_focus_wrapping(configGrammar::Force_focus_wrappingContext *ctx) override {
        auto value = replace_var(unquote(ctx->STRING()->getText()));
        applier.force_focus_wrapping(value);
    }

    void enterMouse_warping(configGrammar::Mouse_warpingContext *ctx) override {
        auto value = replace_var(unquote(ctx->STRING()->getText()));
        applier.mouse_warping(value);
    }

    void enterFocus_follows_mouse(configGrammar::Focus_follows_mouseContext *ctx) override {
        auto value = replace_var(unquote(ctx->STRING()->getText()));
        applier.focus_follows_mouse(value);
    }

    void enterNo_focus(configGrammar::No_focusContext *ctx) override {
        auto match = handle_criteria(ctx->criteria());
        applier.no_focus(match);
    }

    void enterAssign(configGrammar::AssignContext *ctx) override {
        /*
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
         */
    }

    void enterFor_window(configGrammar::For_windowContext *ctx) override {
        auto arguments = ctx->commands()->command()
                         % fn::transform([](auto *x) { return toString(x); })
                         % fn::to_vector();

        auto match = handle_criteria(ctx->criteria());
        for (auto argument : arguments) {
            applier.for_window(match, replace_var(std::move(argument)));
        }
    }

    void enterHide_edge_borders(configGrammar::Hide_edge_bordersContext *ctx) override {
        auto border_type = replace_var(unquote(ctx->STRING()->getText()));
        applier.hide_edge_borders(border_type);
    }

    void enterDefault_border(configGrammar::Default_borderContext *ctx) override {
        /*
        auto defaultBorderType = replace_var(ctx->default_border_type()->getText());
        auto width = replace_var(ctx->border_style()->NUMBER()->getText()); // TODO check for null?
        auto border = replace_var(ctx->border_style()->children[0]->getText());
        applier.default_border(defaultBorderType, border, std::stol(width));
         */
    }

    void enterWorkspace_layout(configGrammar::Workspace_layoutContext *ctx) override {
        auto value = replace_var(unquote(ctx->STRING()->getText()));
        applier.workspace_layout(value);
    }

    void enterDefault_orientation(configGrammar::Default_orientationContext *ctx) override {
        auto value = replace_var(unquote(ctx->STRING()->getText()));
        applier.default_orientation(value);
    }

    void enterFloating_modifier(configGrammar::Floating_modifierContext *ctx) override {
        auto arguments = replace_var(unquote(ctx->STRING()->getText()));

        applier.floating_modifier(arguments);
    }

    void enterFloating_minimum_size(configGrammar::Floating_minimum_sizeContext *ctx) override {
        auto w = replace_var(ctx->dimension()->NUMBER(0)->getText());
        auto h = replace_var(ctx->dimension()->NUMBER(1)->getText());
        applier.floating_minimum_size(std::stol(w), std::stol(h));
    }

    void enterFloating_maximum_size(configGrammar::Floating_maximum_sizeContext *ctx) override {
        auto w = replace_var(ctx->dimension()->NUMBER(0)->getText());
        auto h = replace_var(ctx->dimension()->NUMBER(1)->getText());
        applier.floating_maximum_size(std::stol(w), std::stol(h));
    }

    void enterFont(configGrammar::FontContext *ctx) override {
        auto arguments = replace_var(unquote(ctx->STRING()->getText()));
        applier.font(arguments);
    }

    void enterBinding(configGrammar::BindingContext *ctx) override {
        auto bindtype = replace_var(ctx->children[0]->getText());

        auto options = ctx->OPTION()
                % fn::transform([](auto opt) { return opt->getText(); })
                % fn::unique_all()
                % fn::to_vector();

        auto commands = ctx->commands()->command()
                       % fn::transform([](auto *x) { return toString(x); })
                       % fn::to_vector();

        auto release = options
                % fn::exists_where([](auto text) { return text == "--release"; });

        auto border = options % fn::exists_where([](auto text) { return text == "--border"; });

        auto whole_window = options % fn::exists_where([](auto text) { return text == "--whole-window"; });

        auto exclude_titlebar = options % fn::exists_where([](auto text) { return text == "--exclude-titlebar"; });

        std::string modifiers;
        std::string key;

        std::string keybinding = replace_var(unquote(ctx->STRING()->getText()));

        auto pos = keybinding.find_last_of('+');

        if (pos != string::npos) {
            modifiers = keybinding.substr(0, pos);
            key = keybinding.substr(pos + 1);
        } else {
            key = keybinding;
        }

        if (inMode) {
            for (auto command : commands) {
                applier.mode_binding(bindtype, modifiers, key, release, border, whole_window, exclude_titlebar, replace_var(std::move(command)));
            }
        } else {
            for (auto command : commands) {
                applier.binding(bindtype, modifiers, key, release, border, whole_window, exclude_titlebar, replace_var(std::move(command)));
            }
        }
    }

    void enterEveryRule(antlr4::ParserRuleContext *ctx) override {}

    void visitTerminal(antlr4::tree::TerminalNode *node) override {}

    void visitErrorNode(antlr4::tree::ErrorNode *node) override {
        cout << "ERROR: " << node->getText() << endl;
        throw std::runtime_error("error");
    }

};

NewParser::NewParser(const ResourceDatabase& resourceDatabase, std::istream *stream, config_load_t load_type, BaseConfigApplier &applier) : BaseParser(applier), stream(stream), load_type(load_type), resourceDatabase(resourceDatabase) {

}

class ErrorListener : public BaseErrorListener {
    void syntaxError(Recognizer *recognizer, Token * offendingSymbol, size_t line, size_t charPositionInLine,
                             const std::string &msg, std::exception_ptr e) override {
        std::cout << "ERROR " << fmt::sprintf("(%d:%d): ", line, charPositionInLine) << msg << std::endl;
    }

};

void NewParser::parse_file() {

    ANTLRInputStream input{*stream};
    configLexer lexer{&input};

    lexer.removeErrorListeners();
    ErrorListener pListener{};
    lexer.addErrorListener(&pListener);

    CommonTokenStream tokens{&lexer};
    configGrammar parser{&tokens};

    //parser.setErrorHandler(new BailErrorStrategy());
    parser.removeErrorListeners();
    parser.addErrorListener(&pListener);

    auto tree = parser.config();

    if (parser.getNumberOfSyntaxErrors() > 0) {
        throw std::runtime_error("we got a problem, sir");
    }

    //cout << tree->toStringTree(&parser, true) << endl << endl;

    // 1st pass, store all variables
    VariableListener variableListener{resourceDatabase};
    tree::ParseTreeWalker::DEFAULT.walk(&variableListener, tree);

    // 2nd pass, execute instructions

    StatementListener listener{applier, std::move(variableListener.variables)};
    tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);
}
