module;
class OldParser;
struct criteria_state;
export module i3_config_old:parser_ctx;

import std;
import i3_config_base;
import :parser_stack;

export {
    /**
     * Holds a user-assigned variable for parsing the configuration file. The key
     * is replaced by value in every following line of the file.
     *
     */
    class Variable {
       public:
        std::string key{};
        std::string value{};
    };

    class parser_ctx {
       public:
        int state;
        criteria_state *criteria_state;
        config_load_t load_type;

        /* A list which contains the states that lead to the current state, e.g.
         * INITIAL, WORKSPACE_LAYOUT.
         * When jumping back to INITIAL, statelist_idx will simply be set to 1
         * (likewise for other states, e.g. MODE or BAR).
         * This list is used to process the nearest error token. */
        int statelist[10];
        /* NB: statelist_idx points to where the next entry will be inserted */
        int statelist_idx;

        /*******************************************************************************
         * The (small) stack where identified literals are stored during the parsing
         * of a single config directive (like $workspace).
         ******************************************************************************/
        stack stack{};

        std::vector<std::shared_ptr<Variable>> variables{};

        bool has_errors{false};

        parser_ctx() = delete;

        explicit parser_ctx(OldParser *parser);

        ~parser_ctx();
    };
}