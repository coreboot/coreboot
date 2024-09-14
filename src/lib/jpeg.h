/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __JPEG_H
#define __JPEG_H

#include <stdlib.h>

/* These functions return NULL on success and a short error message on
 * failure. Callers should not free the returned pointer. */

const char *jpeg_fetch_size(unsigned char *filedata, size_t filesize, unsigned int *width,
			    unsigned int *height);
const char *jpeg_decode(unsigned char *filedata, size_t filesize, unsigned char *framebuffer,
			unsigned int width, unsigned int height, unsigned int bytes_per_line,
			unsigned int depth);

#endif
