export module i3_commands_old:cmdp_token;

import std;
import :enums;

struct cmdp_token {
    std::string name;
    std::optional<std::string> identifier;
    /* This might be __CALL */
    cmdp_state next_state;
    std::uint16_t call_identifier;
};
