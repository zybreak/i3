module;
#include <deque>
export module i3:criteria_state;

import :match;

class Con;

export struct criteria_state {
    int criteria_next_state{};
    Match current_match{};
    std::deque<Con*> owindows{};

    criteria_state() = default;

    criteria_state(const criteria_state &src) {
        this->criteria_next_state = src.criteria_next_state;
        this->current_match = src.current_match;
        this->owindows = src.owindows;

    }

    criteria_state(criteria_state &&src) noexcept {
        std::swap(this->criteria_next_state, src.criteria_next_state);
        std::swap(this->current_match, src.current_match);
        std::swap(this->owindows, src.owindows);
    }

    criteria_state& operator=(criteria_state &&src) noexcept {
        std::swap(this->criteria_next_state, src.criteria_next_state);
        std::swap(this->current_match, src.current_match);
        std::swap(this->owindows, src.owindows);
        return *this;
    }
};
