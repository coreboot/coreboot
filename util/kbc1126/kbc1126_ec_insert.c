/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Iru Cai <mytbk920423@gmail.com>
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

#include <stdio.h>
#include <stdlib.h>

static void usage(const char *s)
{
	printf("%s <rom file> <fw1> <fw2> <fw1 offset> <fw2 offset>\n", s);
	exit(1);
}

static void FseekEnd(FILE *fp, long o)
{
	if (fseek(fp, o, SEEK_END) != 0) {
		puts("fseek() error!\n");
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	if (argc < 6)
		usage(argv[0]);

	FILE *fp = fopen(argv[1], "rb+");
	FILE *fw1 = fopen(argv[2], "rb");
	FILE *fw2 = fopen(argv[3], "rb");
	long offset1 = strtol(argv[4], NULL, 0);
	long offset2 = strtol(argv[5], NULL, 0);

	if (fp == NULL || fw1 == NULL || fw2 == NULL) {
		puts("Error opening file!");
		exit(1);
	}

	if ((offset1 & 0xff) || (offset2 & 0xff)) {
		puts("The offsets must be aligned to 0x100");
		exit(1);
	}

	long romsz;
	FseekEnd(fp, -1);
	romsz = ftell(fp) + 1;
	printf("size of %s: 0x%lx\n", argv[1], romsz);

	if (romsz & 0xff) {
		puts("The ROM size must be multiple of 0x100");
		exit(1);
	}

	if (offset1 > 0)
		offset1 = offset1 - romsz;

	if (offset2 > 0)
		offset2 = offset2 - romsz;

	/* write two offsets to $s-0x100 */
	char offs[8];
	long os;
	os = 0x1000000 + offset1;
	offs[0] = os >> 16;
	offs[1] = os >> 8;
	offs[2] = 0xff - offs[0];
	offs[3] = 0xff - offs[1];
	os = 0x1000000 + offset2;
	offs[4] = os >> 16;
	offs[5] = os >> 8;
	offs[6] = 0xff - offs[4];
	offs[7] = 0xff - offs[5];
	for (size_t i = 0; i < 8; i++) {
		printf("%02hhx ", offs[i]);
	}
	puts("");
	FseekEnd(fp, -0x100);
	printf("writing to 0x%lx\n", ftell(fp));
	fwrite(offs, 1, 8, fp);

	/* write fw1 and fw2 */
	char c;
	FseekEnd(fp, offset1);
	printf("writing to 0x%lx\n", ftell(fp));
	while (fread(&c, 1, 1, fw1) == 1) {
		fwrite(&c, 1, 1, fp);
	}
	FseekEnd(fp, offset2);
	printf("writing to 0x%lx\n", ftell(fp));
	while (fread(&c, 1, 1, fw2) == 1) {
		fwrite(&c, 1, 1, fp);
	}

	fclose(fp);
	fclose(fw1);
	fclose(fw2);
	return 0;
}
