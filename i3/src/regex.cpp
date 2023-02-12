/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * regex.c: Interface to libPCRE (perl compatible regular expressions).
 *
 */

#include <cstdlib>
#include <cstring>

#include "libi3.h"
#include "util.h"
#include "regex.h"

/*
 * Creates a new 'regex' struct containing the given pattern and a PCRE
 * compiled regular expression. Also, calls pcre_study because this regex will
 * most likely be used often (like for every new window and on every relevant
 * property change of existing windows).
 *
 * Returns NULL if the pattern could not be compiled into a regular expression
 * (and ELOGs an appropriate error message).
 *
 */
Regex::Regex(const char *pattern) {
    const char *error;
    int errorcode, offset;

    this->pattern = sstrdup(pattern);
    int options = PCRE_UTF8;
    /* We use PCRE_UCP so that \B, \b, \D, \d, \S, \s, \W, \w and some POSIX
     * character classes play nicely with Unicode */
    options |= PCRE_UCP;
    while (!(this->regex = pcre_compile2(pattern, options, &errorcode, &error, &offset, nullptr))) {
        /* If the error is that PCRE was not compiled with UTF-8 support we
         * disable it and try again */
        if (errorcode == 32) {
            options &= ~PCRE_UTF8;
            continue;
        }
        ELOG(fmt::sprintf("PCRE regular expression compilation failed at %d: %s\n",
             offset, error));
        return;
    }
    this->extra = pcre_study(this->regex, 0, &error);
    /* If an error happened, we print the error message, but continue.
     * Studying the regular expression leads to faster matching, but it’s not
     * absolutely necessary. */
    if (error) {
        ELOG(fmt::sprintf("PCRE regular expression studying failed: %s\n",  error));
    } else {
        this->valid = true;
    }
}

Regex::Regex(const Regex &other) : Regex(other.pattern) {
}

Regex::Regex(Regex &&other) noexcept {
    std::swap(this->regex, other.regex);
    std::swap(this->extra, other.extra);
    std::swap(this->valid, other.valid);
    std::swap(this->pattern, other.pattern);
}

/*
 * Frees the given regular expression. It must not be used afterwards!
 *
 */
Regex::~Regex() {
    FREE(this->pattern);
    FREE(this->regex);
    FREE(this->extra);
}

/*
 * Checks if the given regular expression matches the given input and returns
 * true if it does. In either case, it logs the outcome using LOG(), so it will
 * be visible without debug logging.
 *
 */
bool Regex::regex_matches(const char *input) {
    int rc;

    if (!this->valid) {
        return false;
    }

    /* We use strlen() because pcre_exec() expects the length of the input
     * string in bytes */
    if ((rc = pcre_exec(this->regex, this->extra, input, strlen(input), 0, 0, nullptr, 0)) == 0) {
         LOG(fmt::sprintf("Regular expression \"%s\" matches \"%s\"\n",
            this->pattern, input));
        return true;
    }

    if (rc == PCRE_ERROR_NOMATCH) {
         LOG(fmt::sprintf("Regular expression \"%s\" does not match \"%s\"\n",
            this->pattern, input));
        return false;
    }

     ELOG(fmt::sprintf("PCRE error %d while trying to use regular expression \"%s\" on input \"%s\", see pcreapi(3)\n",
         rc, this->pattern, input));
    return false;
}
