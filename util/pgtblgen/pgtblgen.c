/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <getopt.h>

static void usage(char *argv[])
{
	printf("usage: %s -b <addr> -a <arch> -o <file>\n", argv[0]);
	printf("  -a\t architecture. Supported: x86_64\n");
	printf("  -b\t base address\n");
	printf("  -o\t the file to write to\n");
	printf("  -h\t show this help text\n");
}

/*
 * For reference see "AMD64 ArchitectureProgrammer's Manual Volume 2",
 * Document 24593-Rev. 3.31-July 2019 Chapter 5.3.4
 *
 * Page table attributes: WB, User+Supervisor, Present, Writeable, Accessed, Dirty
 */
#define _PRES (1ULL << 0)
#define _RW   (1ULL << 1)
#define _US   (1ULL << 2)
#define _A    (1ULL << 5)
#define _D    (1ULL << 6)
#define _PS   (1ULL << 7)
#define _GEN_DIR(a) (_PRES | _RW | _US | _A | (a))
#define _GEN_PAGE(a) (_PRES | _RW | _US | _PS | _A |  _D | (a))

/*
 * Generate x86_64 page tables.
 * The page tables needs to be placed at @base_address, and identity map
 * the first @size_gib GiB of physical memory.
 */
static int gen_pgtbl_x86_64(const uint64_t base_address,
			    const size_t size_gib,
			    void **out_buf,
			    size_t *out_size)
{
	uint64_t *entry;

	if (!out_size || !out_buf)
		return 1;

	*out_size = (size_gib + 2) * 4096;
	*out_buf = malloc(*out_size);
	if (!*out_buf)
		return 1;

	memset(*out_buf, 0, *out_size);
	entry = (uint64_t *)*out_buf;

	/* Generate one PM4LE entry - point to PDPE */
	entry[0] = _GEN_DIR(base_address + 4096);
	entry += 512;

	/* PDPE table - point to PDE */
	for (size_t i = 0; i < size_gib; i++)
		entry[i] = _GEN_DIR(base_address + 4096 * (i + 2));
	entry += 512;

	/* PDE tables - identity map 2MiB pages */
	for (size_t g = 0; g < size_gib; g++) {
		for (size_t i = 0; i < 512; i++) {
			uint64_t addr = ((1ULL << (12 + 9)) * i) | ((1ULL << (12 + 9 + 9)) * g);
			entry[i] = _GEN_PAGE(addr);
		}
		entry += 512;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int ret = 1;
	uint64_t base_address = 0;
	char *filename = NULL;
	char *arch = NULL;
	void *buf = NULL;
	size_t buf_size = 0;
	int c;

	while ((c = getopt(argc, argv, "ho:a:b:")) != -1)
		switch (c) {
		case '?': /* falltrough */
		case 'h':
			usage(argv);
			return 0;
		case 'o':
			filename = optarg;
			break;
		case 'a':
			arch = optarg;
			break;
		case 'b':
			base_address = strtoull(optarg, NULL, 0);
			break;
		default:
			break;
		}

	if (!filename) {
		fprintf(stderr, "E: Missing filename.\n");
		goto done;
	}
	if (!arch) {
		fprintf(stderr, "E: Missing architecture.\n");
		goto done;
	} else if (strcmp(arch, "x86_64") != 0) {
		fprintf(stderr, "E: Unsupported architecture.\n");
		goto done;
	}
	if (base_address & 4095) {
		fprintf(stderr, "E: Base address not 4 KiB aligned\n");
		goto done;
	}

	/* FIXME: Identity map 4GiB for now, increase if necessary */
	if (strcmp(arch, "x86_64") == 0)
		ret = gen_pgtbl_x86_64(base_address, 4, &buf, &buf_size);

	if (ret) {
		fprintf(stderr, "Failed to generate page tables\n");
		goto done;
	}

	// write the table
	FILE *fd = fopen(filename, "wb");
	if (!fd) {
		fprintf(stderr, "%s open failed: %s\n", filename, strerror(errno));
		goto done;
	}

	if (fwrite(buf, 1, buf_size, fd) != buf_size) {
		fprintf(stderr, "%s write failed: %s\n", filename, strerror(errno));
		fclose(fd);
		goto done;
	}

	if (fclose(fd)) {
		fprintf(stderr, "%s close failed: %s\n", filename, strerror(errno));
		goto done;
	}

	ret = 0;
done:
	free(buf);
	return ret;
}
