export module format_placeholders;

import std;

/** Helper structure for usage in format_placeholders(). */
export struct placeholder_t {
    /* The placeholder to be replaced, e.g., "%title". */
    std::string const name;
    /* The value this placeholder should be replaced with. */
    std::string const value;
};

/*
 * Replaces occurrences of the defined placeholders in the format string.
 *
 */
export std::string format_placeholders(std::string const &format, std::vector<placeholder_t> &placeholders) {
    using namespace std::string_literals;
    if (format.empty()) {
        return ""s;
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
