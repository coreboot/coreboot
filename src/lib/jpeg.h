/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __JPEG_H
#define __JPEG_H

#include <stdlib.h>

#define JPEG_DECODE_FAILED 1

int jpeg_fetch_size(unsigned char *filedata, size_t filesize, unsigned int *width,
		    unsigned int *height);
int jpeg_decode(unsigned char *filedata, size_t filesize, unsigned char *framebuffer,
		unsigned int width, unsigned int height, unsigned int bytes_per_line,
		unsigned int depth);

#endif
