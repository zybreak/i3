/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * A "match" is a data structure which acts like a mask or expression to match
 * certain windows or not. For example, when using commands, you can specify a
 * command like this: [title="*Firefox*"] kill. The title member of the match
 * data structure will then be filled and i3 will check each window using
 * match_matches_window() to find the windows affected by this command.
 *
 */
#include "i3string.h"
#include "log.h"
#include "draw.h"
#include "font.h"
#include "dpi.h"

#include "configuration.h"
#include "con.h"
#include "match.h"
#include "config_parser.h"

import utils;

bool Match::match_is_empty() {
    return false;
}

Match& Match::operator=(Match &&src) noexcept {
    return *this;
}

Match::Match(const Match &src) {
}

bool match_matches_window(Match &match, i3Window *window) {
    LOG(fmt::sprintf("Checking window 0x%08x (class %s)\n",  window->id, window->class_class));
    return false;
}

Match::~Match() {
}

void Match::parse_property(const char *ctype, const char *cvalue) {
    DLOG(fmt::sprintf("ctype=*%s*, cvalue=*%s*\n",  ctype, cvalue));
}

namespace cmd{
    void criteria_init(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output) {
        DLOG(fmt::sprintf("Initializing criteria, current_match = %p\n",  (void*)&criteria_state.current_match));
        criteria_state.current_match = Match();
        criteria_state.owindows.clear();
    }
}

namespace cfg{
    void criteria_init(struct criteria_state &criteria_state, struct ConfigResultIR &result, int _state) {
        criteria_state.criteria_next_state = _state;

        DLOG(fmt::sprintf("Initializing criteria, current_match = %p, state = %d\n",  (void*)&(criteria_state.current_match), _state));
        criteria_state.current_match = Match();
    }

    void criteria_pop_state(struct criteria_state &criteria_state, struct ConfigResultIR &result) {
        result.next_state = criteria_state.criteria_next_state;
    }

    void criteria_add(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *ctype, const char *cvalue) {
        criteria_state.current_match.parse_property(ctype, cvalue);
    }
}