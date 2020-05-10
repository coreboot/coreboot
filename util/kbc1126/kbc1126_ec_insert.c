/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>

static void usage(const char *s)
{
	printf("insert firmware blobs:\n\t"
	       "%s <rom file> <fw1> <fw2> <fw1 offset> <fw2 offset>\n\n",
	       s);
	printf("set addresses of firmware blobs only:\n\t"
	       "%s <rom file> <fw1 offset> <fw2 offset>\n\n",
	       s);
	printf(
	    "offset can be (example is put it to 0x7ffa00 when ROM is 8MB):\n"
	    "- file offset: 0x7ffa00\n"
	    "- distance to the end of file: -0x600\n"
	    "- the address when ROM is mapped to the end of memory: "
	    "0xfffffa00\n");
	exit(1);
}

static void FseekEnd(FILE *fp, long o)
{
	if (fseek(fp, o, SEEK_END) != 0) {
		puts("fseek() error!\n");
		exit(1);
	}
}

static long negoffset(long a, long romsz)
{
	if (a > 0) {
		if (a & 0x80000000) /* the address in memory, and sizeof(long)
				       is 8 */
			return a - 0x100000000;
		else /* the file offset */
			return a - romsz;
	} else {
		return a;
	}
}

int main(int argc, char *argv[])
{
	FILE *fp, *fw1, *fw2;
	long offset1, offset2;

	if (argc != 4 && argc != 6)
		usage(argv[0]);

	fp = fopen(argv[1], "rb+");
	if (fp == NULL) {
		puts("Error opening firmware image!");
		exit(1);
	}

	if (argc == 6) {
		fw1 = fopen(argv[2], "rb");
		fw2 = fopen(argv[3], "rb");
		offset1 = strtoul(argv[4], NULL, 0);
		offset2 = strtoul(argv[5], NULL, 0);

		if (fw1 == NULL || fw2 == NULL) {
			puts("Error opening file!");
			exit(1);
		}
	} else {
		fw1 = NULL;
		fw2 = NULL;
		offset1 = strtoul(argv[2], NULL, 0);
		offset2 = strtoul(argv[3], NULL, 0);
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

	offset1 = negoffset(offset1, romsz);
	offset2 = negoffset(offset2, romsz);

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
	for (size_t i = 0; i < 8; i++)
		printf("%02hhx ", offs[i]);

	puts("");
	FseekEnd(fp, -0x100);
	printf("writing to 0x%lx\n", ftell(fp));
	fwrite(offs, 1, 8, fp);

	if (argc == 6) {
		/* write fw1 and fw2 */
		char c;
		FseekEnd(fp, offset1);
		printf("writing to 0x%lx\n", ftell(fp));
		while (fread(&c, 1, 1, fw1) == 1)
			fwrite(&c, 1, 1, fp);

		FseekEnd(fp, offset2);
		printf("writing to 0x%lx\n", ftell(fp));
		while (fread(&c, 1, 1, fw2) == 1)
			fwrite(&c, 1, 1, fp);

		fclose(fw1);
		fclose(fw2);
	}

	fclose(fp);
	return 0;
}
