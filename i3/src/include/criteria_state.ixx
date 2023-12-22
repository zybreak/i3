export module i3:criteria_state;

export struct criteria_state {
    int criteria_next_state;
    Match current_match{};
    std::deque<Con*> owindows{};
};
