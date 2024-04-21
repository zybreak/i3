module;
#include <string>
#include <deque>
#include <vector>
#include <cstring>
export module i3:format_placeholders;

/** Helper structure for usage in format_placeholders(). */
export struct placeholder_t {
    /* The placeholder to be replaced, e.g., "%title". */
    const std::string name;
    /* The value this placeholder should be replaced with. */
    const std::string value;
};

/*
 * Replaces occurrences of the defined placeholders in the format string.
 *
 */
export std::string format_placeholders(const std::string &format, std::vector<placeholder_t> &placeholders) {
    if (format.empty()) {
        return "";
    }

    /* Now we can parse the format string. */
    std::string buffer{};
    buffer.reserve(format.length());
    
    for (auto walk = format.begin(); walk < format.end(); walk++) {
        if (*walk != '%') {
            buffer += *walk;
            continue;
        }

        bool matched = false;
        for (auto &placeholder : placeholders) {
            auto it = std::search(walk, format.end(), placeholder.name.begin(), placeholder.name.end());
            if (it == format.end()) {
                continue;
            }

            matched = true;
            buffer += placeholder.value;
            std::advance(walk, placeholder.name.size() - 1);
            break;
        }

        if (!matched) {
            buffer += *walk;
        }
    }

    return buffer;
}
