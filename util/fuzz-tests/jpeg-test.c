/* SPDX-License-Identifier: GPL-2.0-only */

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

	unsigned char *buf = malloc(len);
	if (fread(buf, len, 1, f) != 1)
		return 1;
	fclose(f);

	unsigned int width;
	unsigned int height;
	if (jpeg_fetch_size(buf, len, &width, &height) != 0) {
		return 1;
	}
	if ((width > 6000) || (height > 6000)) {
		// infeasible data set
		return 1;
	}
	//printf("width: %d, height: %d\n", width, height);
	unsigned char *pic = malloc(depth / 8 * width * height);
	int ret = jpeg_decode(buf, len, pic, width, height, width * depth / 8, depth);
	//printf("ret: %x\n", ret);
	return ret;
}
