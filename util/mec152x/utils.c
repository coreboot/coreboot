/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "utils.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Return CRC-8 of the data, using x^8 x^2 x 1 polynomial. */
uint8_t crc8_itu(const uint8_t *data, int len)
{
	unsigned int crc = 0;
	int i, j;

	for (j = 0; j < len; j++, data++) {
		crc ^= (*data << 8);
		for (i = 8; i; i--) {
			if (crc & 0x8000)
				crc ^= (0x1070 << 3);
			crc <<= 1;
		}
	}

	return (uint8_t)(crc >> 8) ^ 0x55;
}

struct mem_range map_file(const char path[], bool rw)
{
	struct mem_range range = {0};

	int open_flags = rw ? O_RDWR : O_RDONLY;
	int mmap_flags = rw ? PROT_READ | PROT_WRITE : PROT_READ;

	int fd = open(path, open_flags);
	if (fd == -1) {
		fprintf(stderr, "Failed to open(): %s\n", strerror(errno));
		return range;
	}

	struct stat stat_buf;
	if (fstat(fd, &stat_buf) != 0) {
		(void)close(fd);
		fprintf(stderr, "Failed to fstat(): %s\n", strerror(errno));
		return range;
	}

	if (stat_buf.st_size == 0) {
		(void)close(fd);
		fprintf(stderr, "Can't map an empty \"%s\" file\n", path);
		return range;
	}

	uint8_t *mem = mmap(/*addr=*/NULL, stat_buf.st_size, mmap_flags,
			    MAP_SHARED | MAP_POPULATE, fd, /*offset=*/0);
	(void)close(fd);
	if (mem == MAP_FAILED) {
		fprintf(stderr, "Failed to mmap(): %s\n", strerror(errno));
		return range;
	}

	range.start = mem;
	range.length = stat_buf.st_size;
	return range;
}

void unmap_file(struct mem_range store)
{
	if (munmap(store.start, store.length) != 0)
		fprintf(stderr, "Failed to munmap(): %s\n", strerror(errno));
}
