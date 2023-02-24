#ifndef __BASE_PARSER
#define __BASE_PARSER

enum parse_file_result_t {
    PARSE_FILE_FAILED = -1,
    PARSE_FILE_SUCCESS = 0,
    PARSE_FILE_CONFIG_ERRORS = 1,
};

    /*
     * A utility function to convert a string containing the group and modifiers to
     * the corresponding bit mask.
     */
    inline i3_event_state_mask_t event_state_from_str(const char *str) {
        /* It might be better to use strtok() here, but the simpler strstr() should
         * do for now. */
        i3_event_state_mask_t result = 0;
        if (str == nullptr || strcmp(str, "") == 0)
            return result;
        if (strstr(str, "Mod1") != nullptr)
            result |= XCB_KEY_BUT_MASK_MOD_1;
        if (strstr(str, "Mod2") != nullptr)
            result |= XCB_KEY_BUT_MASK_MOD_2;
        if (strstr(str, "Mod3") != nullptr)
            result |= XCB_KEY_BUT_MASK_MOD_3;
        if (strstr(str, "Mod4") != nullptr)
            result |= XCB_KEY_BUT_MASK_MOD_4;
        if (strstr(str, "Mod5") != nullptr)
            result |= XCB_KEY_BUT_MASK_MOD_5;
        if (strstr(str, "Control") != nullptr ||
            strstr(str, "Ctrl") != nullptr)
            result |= XCB_KEY_BUT_MASK_CONTROL;
        if (strstr(str, "Shift") != nullptr)
            result |= XCB_KEY_BUT_MASK_SHIFT;

        if (strstr(str, "Group1") != nullptr)
            result |= (I3_XKB_GROUP_MASK_1 << 16);
        if (strstr(str, "Group2") != nullptr ||
            strstr(str, "Mode_switch") != nullptr)
            result |= (I3_XKB_GROUP_MASK_2 << 16);
        if (strstr(str, "Group3") != nullptr)
            result |= (I3_XKB_GROUP_MASK_3 << 16);
        if (strstr(str, "Group4") != nullptr)
            result |= (I3_XKB_GROUP_MASK_4 << 16);
        return result;
    }

class BaseParser {
public:
    virtual ~BaseParser() = default;
    virtual parse_file_result_t parse_file() = 0;
};

#endif