#pragma once

#include <sys/types.h>

/**
 * Emulates mkdir -p (creates any missing folders)
 *
 */
int mkdirp(const char *path, mode_t mode);
