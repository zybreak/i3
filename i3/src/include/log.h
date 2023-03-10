#pragma once

#include <iostream>
#include <string>
#include <fmt/core.h>
#include <fmt/printf.h>

/**
 * Checks if debug logging is active.
 *
 */
bool get_debug_logging();

/**
 * Set debug logging.
 *
 */
void set_debug_logging(const bool _debug_logging);

bool get_verbosity();

/**
 * Set verbosity of i3. If verbose is set to true, informative messages will
 * be printed to stdout. If verbose is set to false, only errors will be
 * printed.
 *
 */
void set_verbosity(bool _verbose);

/*
 * Logs the given message to stdout while prefixing the current time to it,
 * but only if verbose mode is activated.
 *
 */
inline __attribute__((always_inline))
void LOG(const std::string_view &msg) {
    if (!get_verbosity())
        return;

    std::cout << msg;
}

/*
 * Logs the given message to stdout while prefixing the current time to it.
 *
 */
inline __attribute__((always_inline))
void ELOG(const std::string_view &msg) {
    std::cerr << fmt::format("ERROR: {}", msg);
}

/*
 * Logs the given message to stdout while prefixing the current time to it,
 * but only if debug logging was activated.
 * This is to be called by DLOG() which includes filename/linenumber
 *
 */
inline __attribute__((always_inline))
void DLOG(const std::string_view &msg) {
    if (!(get_debug_logging()))
        return;

    std::cerr << fmt::format("DEBUG: {}", msg);
}
