/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

void *memalign(size_t boundary, size_t size);
void *malloc(size_t size);
void *calloc(size_t nitems, size_t size);
void free(void *ptr);

#endif /* STDLIB_H */
