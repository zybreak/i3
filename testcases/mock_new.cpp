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
#include "libi3.h"
#include "configuration.h"
#include "con.h"
#include "match.h"
#include "config_parser.h"

bool match_is_empty(Match &match) {
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
