#pragma once

#include <string>

/**
 * This function resolves ~ in pathnames.
 * It may resolve wildcards in the first part of the path, but if no match
 * or multiple matches are found, it just returns a copy of path as given.
 *
 */
std::string resolve_tilde(const std::string_view &path);
