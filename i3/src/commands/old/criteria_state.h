#pragma once

#include "match.h"
#include "con.h"

struct criteria_state {
    int criteria_next_state;
    Match current_match{};
    std::deque<Con*> owindows{};
};
