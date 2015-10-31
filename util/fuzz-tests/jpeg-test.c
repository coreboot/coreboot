/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdlib.h>
#include <stdio.h>
#include "jpeg.h"

const int depth = 16;

int main(int argc, char **argv)
{
	FILE *f = fopen(argv[1], "rb");
	unsigned long len;

	if (!f)
		return 1;
	if (fseek(f, 0, SEEK_END) != 0)
		return 1;
	len = ftell(f);
	if (fseek(f, 0, SEEK_SET) != 0)
		return 1;

	char *buf = malloc(len);
	struct jpeg_decdata *decdata = malloc(sizeof(*decdata));
	if (fread(buf, len, 1, f) != 1)
		return 1;
	fclose(f);

	int width;
	int height;
	jpeg_fetch_size(buf, &width, &height);
	//printf("width: %d, height: %d\n", width, height);
	char *pic = malloc(depth / 8 * width * height);
	int ret = jpeg_decode(buf, pic, width, height, depth, decdata);
	//printf("ret: %x\n", ret);
	return ret;
}
