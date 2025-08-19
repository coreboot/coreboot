/* SPDX-License-Identifier: GPL-2.0-only */

/**
 * Note: This file is only for POSIX compatibility.
 */

#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>

int __printf(3, 4) snprintf(char *buf, size_t size, const char *fmt, ...);

#endif /* STDIO_H */
