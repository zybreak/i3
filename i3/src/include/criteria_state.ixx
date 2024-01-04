module;
#include <deque>
export module i3:criteria_state;

import :match;

class Con;

export struct criteria_state {
    int criteria_next_state;
    Match current_match{};
    std::deque<Con*> owindows{};
};
