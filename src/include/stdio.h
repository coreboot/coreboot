/* SPDX-License-Identifier: GPL-2.0-only */

/**
 * Note: This file is only for POSIX compatibility, and is meant to be
 * chain-included via string.h.
 */

#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>

int snprintf(char *buf, size_t size, const char *fmt, ...);

#endif /* STDIO_H */
