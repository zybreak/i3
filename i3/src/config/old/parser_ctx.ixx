module;
struct criteria_state;
#include <vector>
export module i3_config_old:parser_ctx;

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
        char *key;
        char *value;

        ~Variable();
    };

    class parser_ctx {
       public:
        BaseResourceDatabase &resourceDatabase;
        std::vector<std::string> included_files{};
        config_load_t load_type;
        BaseConfigApplier &applier;
        int state;
        criteria_state *criteria_state;

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

        parser_ctx(BaseConfigApplier &applier, BaseResourceDatabase &resourceDatabase, config_load_t load_type)
            : applier(applier), load_type(load_type), resourceDatabase(resourceDatabase) {
            this->criteria_state = applier.criteria_init(0);
        }

        ~parser_ctx() = default;
    };
}