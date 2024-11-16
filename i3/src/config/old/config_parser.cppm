/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * config_parser.h: config parser-related definitions
 *
 */
module;
struct criteria_state;
export module i3_config_old:config_parser;

import std;
import i3_config_base;
import :parser_stack;
import :parser_ctx;
import :cmdp_token;

export {
    /**
     * Parses the given file by first replacing the variables, then calling
     * parse_config and launching i3-nagbar if use_nagbar is true.
     *
     * The return value is a boolean indicating whether there were errors during
     * parsing.
     *
     */
    class OldParser : public BaseParser {
      private:
        std::filesystem::path const filename;
        std::string old_dir{};
        std::istream &stream;

        bool parse_config(std::string const &input, std::ostream &err_output);
        void unhandled_token(std::string const &input, int linecnt, std::vector<cmdp_token> const &ptr, bool &has_errors, std::string::const_iterator &walk, std::ostream &err_output);
        bool handle_literal(std::string::const_iterator &walk, cmdp_token const &token);
        bool handle_number(std::string::const_iterator &walk, cmdp_token const &token);
        bool handle_word(std::string::const_iterator &walk, cmdp_token const &token);
        bool handle_line(std::string::const_iterator &walk, cmdp_token const &token);
        bool handle_end(std::string::const_iterator &walk, cmdp_token const &token, ConfigResultIR &subcommand_output, int *linecnt);

      public:
        parser_ctx *parent_ctx = nullptr;
        parser_ctx ctx;
        OldParser(std::filesystem::path const filename, std::istream &stream, BaseResourceDatabase &resourceDatabase, parser_ctx &parent_ctx, BaseConfigApplier &applier);
        OldParser(std::filesystem::path const filename, std::istream &stream, BaseResourceDatabase &resourceDatabase, BaseConfigApplier &applier);
        ~OldParser() override;
        void parse_file() override;
    };
}