/* common utility functions for cbfstool */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include "common.h"
#include "cbfs.h"

/* Utilities */
int verbose = 0;

static off_t get_file_size(FILE *f)
{
	off_t fsize;
	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	return fsize;
}

/* Buffer and file I/O */
int buffer_create(struct buffer *buffer, size_t size, const char *name)
{
	buffer->name = strdup(name);
	buffer->offset = 0;
	buffer->size = size;
	buffer->data = (char *)malloc(buffer->size);
	if (!buffer->data) {
		fprintf(stderr, "buffer_create: Insufficient memory (0x%zx).\n",
			size);
	}
	return (buffer->data == NULL);
}

int buffer_from_file_aligned_size(struct buffer *buffer, const char *filename,
				  size_t size_granularity)
{
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		perror(filename);
		return -1;
	}
	off_t file_size = get_file_size(fp);
	if (file_size < 0) {
		fprintf(stderr, "could not determine size of %s\n", filename);
		fclose(fp);
		return -1;
	}
	if (buffer_create(buffer, ALIGN_UP(file_size, size_granularity), filename)) {
		fprintf(stderr, "could not allocate buffer\n");
		fclose(fp);
		return -1;
	}
	if (fread(buffer->data, 1, file_size, fp) != (size_t)file_size) {
		fprintf(stderr, "incomplete read: %s\n", filename);
		fclose(fp);
		buffer_delete(buffer);
		return -1;
	}
	fclose(fp);

	if (buffer->size > (size_t)file_size)
		memset(buffer->data + file_size, 0xff, buffer->size - file_size);

	return 0;
}

int buffer_from_file(struct buffer *buffer, const char *filename)
{
	return buffer_from_file_aligned_size(buffer, filename, 1);
}

int buffer_write_file(struct buffer *buffer, const char *filename)
{
	FILE *fp = fopen(filename, "wb");
	if (!fp) {
		perror(filename);
		return -1;
	}
	assert(buffer && buffer->data);
	if (fwrite(buffer->data, 1, buffer->size, fp) != buffer->size) {
		fprintf(stderr, "incomplete write: %s\n", filename);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

void buffer_delete(struct buffer *buffer)
{
	assert(buffer);
	if (buffer->name) {
		free(buffer->name);
		buffer->name = NULL;
	}
	if (buffer->data) {
		free(buffer_get_original_backing(buffer));
		buffer->data = NULL;
	}
	buffer->offset = 0;
	buffer->size = 0;
}

static struct {
	uint32_t arch;
	const char *name;
} arch_names[] = {
	{ CBFS_ARCHITECTURE_AARCH64, "arm64" },
	{ CBFS_ARCHITECTURE_ARM, "arm" },
	{ CBFS_ARCHITECTURE_MIPS, "mips" },
	{ CBFS_ARCHITECTURE_PPC64, "ppc64" },
	/* power8 is a reasonable alias */
	{ CBFS_ARCHITECTURE_PPC64, "power8" },
	{ CBFS_ARCHITECTURE_RISCV, "riscv" },
	{ CBFS_ARCHITECTURE_X86, "x86" },
	{ CBFS_ARCHITECTURE_UNKNOWN, "unknown" }
};

uint32_t string_to_arch(const char *arch_string)
{
	size_t i;
	uint32_t ret = CBFS_ARCHITECTURE_UNKNOWN;

	for (i = 0; i < ARRAY_SIZE(arch_names); i++) {
		if (!strcasecmp(arch_string, arch_names[i].name)) {
			ret = arch_names[i].arch;
			break;
		}
	}

	return ret;
}

const char *arch_to_string(uint32_t a)
{
        size_t i;
	const char *ret = NULL;

	for (i = 0; i < ARRAY_SIZE(arch_names); i++) {
		if (a == arch_names[i].arch) {
			ret = arch_names[i].name;
			break;
		}
	}

	return ret;
}

void print_supported_architectures(void)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(arch_names); i++) {
		printf(i == 0? "  ":", ");
		printf("%s", arch_names[i].name);
	}

	printf("\n");
}

void print_supported_filetypes(void)
{
	int i;

	for (i=0; filetypes[i].name; i++) {
		printf(" %s%c", filetypes[i].name, filetypes[i + 1].name ? ',' : '\n');
		if ((i%8) == 7)
			printf("\n");
	}
}

uint64_t intfiletype(const char *name)
{
	size_t i;
	for (i = 0; filetypes[i].name; i++)
		if (strcmp(filetypes[i].name, name) == 0)
			return filetypes[i].type;
	return -1;
}

char *bintohex(uint8_t *data, size_t len)
{
	static const char translate[16] = "0123456789abcdef";

	char *result = malloc(len * 2 + 1);
	if (result == NULL)
		return NULL;

	result[len*2] = '\0';
	unsigned int i;
	for (i = 0; i < len; i++) {
		result[i*2] = translate[(data[i] >> 4) & 0xf];
		result[i*2+1] = translate[data[i] & 0xf];
	}
	return result;
}
