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

ssize_t write_body(char *output, void *body_offset, ssize_t body_size)
{
	char body_name[PATH_MAX], body_tmp_name[PATH_MAX];
	int ret;
	int fd;
	ssize_t bytes = -1;

	/* Create a tmp file and rename it at the end so that make does not get confused
	   if amdfwtool is killed for some unexpected reasons. */
	ret = snprintf(body_tmp_name, sizeof(body_tmp_name), "%s%s%s",
			output, BODY_FILE_SUFFIX, TMP_FILE_SUFFIX);
	if (ret < 0) {
		fprintf(stderr, "Error %s forming BODY tmp file name: %d\n",
							strerror(errno), ret);
		return -1;
	} else if ((unsigned int)ret >= sizeof(body_tmp_name)) {
		fprintf(stderr, "BODY File name %d  > %zu\n", ret, sizeof(body_tmp_name));
		return -1;
	}

	fd = open(body_tmp_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (fd < 0) {
		fprintf(stderr, "Error: Opening %s file: %s\n", body_tmp_name, strerror(errno));
		return -1;
	}

	bytes = write_from_buf_to_file(fd, body_offset, body_size);
	if (bytes != body_size) {
		fprintf(stderr, "Error: Writing to file %s failed\n", body_tmp_name);
		return -1;
	}
	close(fd);

	/* Rename the tmp file */
	ret = snprintf(body_name, sizeof(body_name), "%s%s", output, BODY_FILE_SUFFIX);
	if (ret < 0) {
		fprintf(stderr, "Error %s forming BODY file name: %d\n", strerror(errno), ret);
		return -1;
	}

	if (rename(body_tmp_name, body_name)) {
		fprintf(stderr, "Error: renaming file %s to %s\n", body_tmp_name, body_name);
		return -1;
	}

	return bytes;
}

ssize_t copy_blob(void *dest, const char *src_file, size_t room)
{
	int fd;
	struct stat fd_stat;
	ssize_t bytes;

	fd = open(src_file, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Error opening file: %s: %s\n",
		       src_file, strerror(errno));
		return -1;
	}

	if (fstat(fd, &fd_stat)) {
		fprintf(stderr, "fstat error: %s\n", strerror(errno));
		close(fd);
		return -2;
	}

	if ((size_t)fd_stat.st_size > room) {
		fprintf(stderr, "Error: %s will not fit.  Exiting.\n", src_file);
		close(fd);
		return -3;
	}

	bytes = read(fd, dest, (size_t)fd_stat.st_size);
	close(fd);
	if (bytes != (ssize_t)fd_stat.st_size) {
		fprintf(stderr, "Error while reading %s\n", src_file);
		return -4;
	}

	return bytes;
}
