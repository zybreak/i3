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
#include <config.h>

#include <string>
#include <vector>
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
        char *old_dir;
        int fd;
        FILE *fstr;

       public:
        char *current_config = nullptr;
        struct parser_ctx *parent_ctx = nullptr;
        struct parser_ctx ctx;
        std::vector<std::string> included_files{};
        OldParser(const char *filename, BaseResourceDatabase &resourceDatabase, struct parser_ctx &parent_ctx, BaseConfigApplier &applier);
        OldParser(const char *filename, BaseResourceDatabase &resourceDatabase, config_load_t load_type, BaseConfigApplier &applier);
        ~OldParser() override;
        void parse_file() override;
    };
}