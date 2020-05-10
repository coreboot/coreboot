/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char *s)
{
	printf("%s <rom file>\n", s);
	exit(1);
}

static void FseekEnd(FILE *fp, long o)
{
	if (fseek(fp, o, SEEK_END) != 0) {
		puts("fseek() error!\n");
		exit(1);
	}
}

void dump_fw(FILE *dst, FILE *src, long offset)
{
	static unsigned char buf[65536];

	if (offset > 0)
		offset -= 0x1000000;

	printf("Dumping firmware at -0x%lx...", -offset);

	FseekEnd(src, offset);
	unsigned short len;
	unsigned short cksum;
	unsigned short _cksum = 0;
	fread(&len, 2, 1, src);
	fread(&cksum, 2, 1, src);
	fread(buf, len, 1, src);

	for (size_t i = 0; i < len; i++)
		_cksum += buf[i];

	if (_cksum == cksum) {
		puts("checksum ok");
	} else {
		puts("checksum fail");
		exit(1);
	}

	fwrite(&len, 2, 1, dst);
	fwrite(&cksum, 2, 1, dst);
	fwrite(buf, len, 1, dst);
}

int main(int argc, char *argv[])
{
	if (argc != 2)
		usage(argv[0]);

	FILE *fp = fopen(argv[1], "rb");

	if (fp == NULL) {
		puts("Error opening file!");
		exit(1);
	}

	char *basename = strrchr(argv[1], '/');
	if (basename == NULL)
		basename = argv[1];
	else
		basename = basename + 1;

	int len = strlen(basename);
	char fn1[len + 5], fn2[len + 5];
	strcpy(fn1, basename);
	strcpy(fn2, basename);
	strcat(fn1, ".fw1");
	strcat(fn2, ".fw2");

	FILE *fw1 = fopen(fn1, "wb");
	FILE *fw2 = fopen(fn2, "wb");

	long romsz;
	FseekEnd(fp, -1);
	romsz = ftell(fp) + 1;
	printf("size of %s: 0x%lx\n", argv[1], romsz);

	if (romsz & 0xff) {
		puts("The ROM size must be multiple of 0x100");
		exit(1);
	}

	/* read offset of fw1 and fw2 */
	unsigned char offs[8];
	FseekEnd(fp, -0x100);
	fread(offs, 8, 1, fp);

	assert(offs[0] + offs[2] == 0xff);
	assert(offs[1] + offs[3] == 0xff);
	assert(offs[4] + offs[6] == 0xff);
	assert(offs[5] + offs[7] == 0xff);
	long offw1 = (offs[0] << 16) | (offs[1] << 8);
	long offw2 = (offs[4] << 16) | (offs[5] << 8);

	dump_fw(fw1, fp, offw1);
	dump_fw(fw2, fp, offw2);

	fclose(fp);
	fclose(fw1);
	fclose(fw2);
	return 0;
}
