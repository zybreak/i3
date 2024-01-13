module;
#include <string>
export module i3:format_placeholders;

export {
/** Helper structure for usage in format_placeholders(). */
struct placeholder_t {
    /* The placeholder to be replaced, e.g., "%title". */
    const std::string name;
    /* The value this placeholder should be replaced with. */
    const std::string value;
};

/*
 * Replaces occurrences of the defined placeholders in the format string.
 *
 */
std::string format_placeholders(const std::string &format, std::vector<placeholder_t> &placeholders);
}