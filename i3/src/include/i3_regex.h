/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * regex.c: Interface to libPCRE (perl compatible regular expressions).
 *
 */
#pragma once

#define PCRE2_CODE_UNIT_WIDTH 8

#include <config.h>
#include <pcre2.h>

/**
 * Regular expression wrapper. It contains the pattern itself as a string (like
 * ^foo[0-9]$) as well as a pointer to the compiled PCRE expression and the
 * pcre_extra data returned by pcre_study().
 *
 * This makes it easier to have a useful logfile, including the matching or
 * non-matching pattern.
 *
 */
class Regex {
    pcre2_code *regex{nullptr};
public:
    bool valid{false};
    char *pattern{nullptr};

    explicit Regex(const char *pattern);
    Regex(const Regex &other);
    Regex(Regex &&other) noexcept;
    Regex& operator=(Regex &&other) noexcept;
    ~Regex();
    bool regex_matches(const char *input);
};

