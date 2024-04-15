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
#include <string>
#include <vector>
#include <cstdio>
#include <memory>
export module i3_config_old:config_parser;

import i3_config_base;
import :parser_stack;
import :parser_ctx;

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
        const char *filename;
        std::string old_dir{};
        std::istream &stream;

       public:
        config_load_t load_type;
        parser_ctx *parent_ctx = nullptr;
        parser_ctx ctx;
        std::vector<std::unique_ptr<IncludedFile>> included_files{};
        OldParser(const char *filename, std::istream &stream, BaseResourceDatabase &resourceDatabase, parser_ctx &parent_ctx, BaseConfigApplier &applier);
        OldParser(const char *filename, std::istream &stream, BaseResourceDatabase &resourceDatabase, config_load_t load_type, BaseConfigApplier &applier);
        ~OldParser() override;
        void parse_file() override;
    };
}