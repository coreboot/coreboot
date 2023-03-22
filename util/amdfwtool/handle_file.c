/* SPDX-License-Identifier: GPL-2.0-only */

#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "amdfwtool.h"

void write_or_fail(int fd, void *ptr, size_t size)
{
	ssize_t written;

	written = write_from_buf_to_file(fd, ptr, size);
	if (written < 0 || (size_t)written != size) {
		fprintf(stderr, "%s: Error writing %zu bytes - written %zd bytes\n",
								__func__, size, written);
		exit(-1);
	}
}

ssize_t read_from_file_to_buf(int fd, void *buf, size_t buf_size)
{
	ssize_t bytes;
	size_t total_bytes = 0;

	do {
		bytes = read(fd, buf + total_bytes, buf_size - total_bytes);
		if (bytes == 0) {
			fprintf(stderr, "Reached EOF probably\n");
			break;
		}

		if (bytes < 0 && errno == EAGAIN)
			bytes = 0;

		if (bytes < 0) {
			fprintf(stderr, "Read failure %s\n", strerror(errno));
			return bytes;
		}

		total_bytes += bytes;
	} while (total_bytes < buf_size);

	if (total_bytes != buf_size) {
		fprintf(stderr, "Read data size(%zu) != buffer size(%zu)\n",
								total_bytes, buf_size);
		return -1;
	}
	return buf_size;
}

ssize_t write_from_buf_to_file(int fd, const void *buf, size_t buf_size)
{
	ssize_t bytes;
	size_t total_bytes = 0;

	do {
		bytes = write(fd, buf + total_bytes, buf_size - total_bytes);
		if (bytes < 0 && errno == EAGAIN)
			bytes = 0;

		if (bytes < 0) {
			fprintf(stderr, "Write failure %s\n", strerror(errno));
			lseek(fd, SEEK_CUR, -total_bytes);
			return bytes;
		}

		total_bytes += bytes;
	} while (total_bytes < buf_size);

	if (total_bytes != buf_size) {
		fprintf(stderr, "Wrote more data(%zu) than buffer size(%zu)\n",
								total_bytes, buf_size);
		lseek(fd, SEEK_CUR, -total_bytes);
		return -1;
	}

	return buf_size;
}
