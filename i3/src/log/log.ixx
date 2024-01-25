module;

#include <iostream>
#include <string>
#include <fmt/core.h>
#include <fmt/printf.h>
#include <spdlog/spdlog.h>
#include <source_location>
#include <string_view>

export module log;

static bool debug_logging = false;
static bool verbose = false;

// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

static inline std::string trim(std::string_view s) {
    std::string c = std::string{s.data()};
    ltrim(c);
    rtrim(c);

    return c;
}

export {

    /**
    * Checks if debug logging is active.
    *
    */
    bool get_debug_logging() {
        return debug_logging;
    }

    /**
     * Set debug logging.
     *
     */
    void set_debug_logging(const bool _debug_logging) {
        debug_logging = _debug_logging;

        if (debug_logging) {
            spdlog::set_level(spdlog::level::debug);
        } else if (verbose) {
            spdlog::set_level(spdlog::level::info);
        } else {
            spdlog::set_level(spdlog::level::err);
        }
    }

    bool get_verbosity() {
        return verbose;
    }

    /**
     * Set verbosity of i3. If verbose is set to true, informative messages will
     * be printed to stdout. If verbose is set to false, only errors will be
     * printed.
     *
     */
    void set_verbosity(bool _verbose) {
        verbose = _verbose;

        if (debug_logging) {
            spdlog::set_level(spdlog::level::debug);
        } else if (verbose) {
            spdlog::set_level(spdlog::level::info);
        } else {
            spdlog::set_level(spdlog::level::err);
        }
    }

    void set_formatting() {
        spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e %z %^%8l%$ [thread %t] %-20s : %v");
    }

    /**
     * Logs the given message to stdout while prefixing the current time to it,
     * but only if verbose mode is activated.
     */
    void LOG(std::string_view msg, std::source_location loc = std::source_location::current()) {
        //spdlog::info(msg);
        //SPDLOG_INFO(msg);
        spdlog::default_logger_raw()->log(spdlog::source_loc{loc.file_name(), (int)loc.line(), loc.function_name()}, spdlog::level::info, trim(msg));
    }

    /**
     * Logs the given message to stdout while prefixing the current time to it.
     */
    void ELOG(std::string_view msg, std::source_location loc = std::source_location::current()) {
        //spdlog::error(msg);
        //SPDLOG_ERROR(msg);
        spdlog::default_logger_raw()->log(spdlog::source_loc{loc.file_name(), (int)loc.line(), loc.function_name()}, spdlog::level::err, trim(msg));
    }

    /**
     * Logs the given message to stdout while prefixing the current time to it,
     * but only if debug logging was activated.
     * This is to be called by DLOG() which includes filename/linenumber
     */
    void DLOG(std::string_view msg, std::source_location loc = std::source_location::current()) {
        //spdlog::debug(msg);
        //SPDLOG_DEBUG(msg);
        spdlog::default_logger_raw()->log(spdlog::source_loc{loc.file_name(), (int)loc.line(), loc.function_name()}, spdlog::level::debug, trim(msg));
    }
}
