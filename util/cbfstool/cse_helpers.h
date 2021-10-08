/* SPDX-License-Identifier: GPL-2.0-only */

#include <stddef.h>

#include "common.h"

#define READ_MEMBER(_buff, _x)		read_member(_buff, &(_x), sizeof(_x))
#define WRITE_MEMBER(_buff, _x)		write_member(_buff, &(_x), sizeof(_x))

/* Write data from `src` of length `size` to buffer in little-endian format and seek the buffer
   forward by `size` bytes. */
void write_member(struct buffer *buff, void *src, size_t size);

/* Read data from buffer in little-endian format of length `size` to `dst` and seek the buffer
   forward by `size` bytes. */
void read_member(struct buffer *buff, void *dst, size_t size);
