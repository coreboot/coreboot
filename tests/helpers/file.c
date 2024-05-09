/* SPDX-License-Identifier: GPL-2.0-only */

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Get the file size of a given file
 *
 * @params fname  name of the file relative to the __TEST_DATA_DIR__ directory
 *
 * @return  On success file size in bytes is returned. On failure -1 is returned.
 */
int test_get_file_size(const char *fname)
{
	char path[strlen(__TEST_DATA_DIR__) + strlen(fname) + 2];
	sprintf(path, "%s/%s", __TEST_DATA_DIR__, fname);

	struct stat st;
	if (stat(path, &st) == -1)
		return -1;
	return st.st_size;
}

/*
 * Read a file and write its contents into a buffer
 *
 * @params fname  name of the file relative to the __TEST_DATA_DIR__ directory
 * @params buf    buffer to write file contents into
 * @params size   size of buf
 *
 * @return  On success number of bytes read is returned. On failure -1 is returned.
 */
int test_read_file(const char *fname, uint8_t *buf, size_t size)
{
	char path[strlen(__TEST_DATA_DIR__) + strlen(fname) + 2];
	sprintf(path, "%s/%s", __TEST_DATA_DIR__, fname);

	int f = open(path, O_RDONLY);
	if (f == -1)
		return -1;

	int read_size = read(f, buf, size);

	close(f);
	return read_size;
}
