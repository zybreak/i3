/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * regex.c: Interface to libPCRE (perl compatible regular expressions).
 *
 */
module;
#define PCRE2_CODE_UNIT_WIDTH 8

#include <config.h>
#include <pcre2.h>
#include <fmt/printf.h>
export module regex;

import std;
import utils;
import log;

export {
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
        std::string pattern{};

        explicit Regex(std::string const pattern);
        Regex(Regex const &other);
        Regex(Regex &&other) noexcept;
        Regex &operator=(Regex &&other) noexcept;
        ~Regex();
        bool regex_matches(std::string_view const input) const;
    };
}

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
Regex::Regex(std::string const pattern)
    : pattern(pattern) {
    int errorcode;
    PCRE2_SIZE offset;

    uint32_t options = PCRE2_UTF;
    /* We use PCRE_UCP so that \B, \b, \D, \d, \S, \s, \W, \w and some POSIX
     * character classes play nicely with Unicode */
    options |= PCRE2_UCP;
    if (!(this->regex = pcre2_compile((PCRE2_SPTR)pattern.c_str(), PCRE2_ZERO_TERMINATED, options, &errorcode, &offset, nullptr))) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
        ELOG(std::format("PCRE regular expression compilation failed at {}: {}",
                         offset, buffer));
        this->valid = false;
        return;
    }
    this->valid = true;
}

Regex::Regex(Regex const &other)
    : Regex(other.pattern) {
}

Regex::Regex(Regex &&other) noexcept {
    std::swap(this->regex, other.regex);
    std::swap(this->valid, other.valid);
    std::swap(this->pattern, other.pattern);
}

Regex &Regex::operator=(Regex &&other) noexcept {
    std::swap(this->regex, other.regex);
    std::swap(this->valid, other.valid);
    std::swap(this->pattern, other.pattern);

    return *this;
}

/*
 * Frees the given regular expression. It must not be used afterwards!
 *
 */
Regex::~Regex() {
    pcre2_code_free(this->regex);
}

/*
 * Checks if the given regular expression matches the given input and returns
 * true if it does. In either case, it logs the outcome using LOG(), so it will
 * be visible without debug logging.
 *
 */
bool Regex::regex_matches(std::string_view const input) const {
    pcre2_match_data *match_data;
    int rc;

    if (!this->valid) {
        return false;
    }

    match_data = pcre2_match_data_create_from_pattern(this->regex, nullptr);

    /* We use strlen() because pcre_exec() expects the length of the input
     * string in bytes */
    rc = pcre2_match(this->regex, (PCRE2_SPTR)input.data(), input.length(), 0, 0, match_data, nullptr);
    pcre2_match_data_free(match_data);
    if (rc > 0) {
        LOG(std::format("Regular expression \"{}\" matches \"{}\"",
                        this->pattern, input));
        return true;
    }

    if (rc == PCRE2_ERROR_NOMATCH) {
        LOG(std::format("Regular expression \"{}\" does not match \"{}\"",
                        this->pattern, input));
        return false;
    }

    ELOG(std::format("PCRE error {} while trying to use regular expression \"{}\" on input \"{}\", see pcreapi(3)\n",
                     rc, this->pattern, input));
    return false;
}
