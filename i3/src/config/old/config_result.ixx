module;
export module i3_config_old:config_result;

import :parser_ctx;

export {
    /**
     * An intermediate reprsentation of the result of a parse_config call.
     * Currently unused, but the JSON output will be useful in the future when we
     * implement a config parsing IPC command.
     *
     */
    class ConfigResultIR {
       public:
        parser_ctx ctx;

        /* The next state to transition to. Passed to the function so that we can
         * determine the next state as a result of a function call, like
         * cfg_criteria_pop_state() does. */
        int next_state;

        /* Whether any error happened while processing this config directive. */
        bool has_errors;
    };
}