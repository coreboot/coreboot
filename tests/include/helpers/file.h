/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _TESTS_HELPERS_FILE_H
#define _TESTS_HELPERS_FILE_H

#include <stddef.h>
#include <stdint.h>

int test_get_file_size(const char *fname);
int test_read_file(const char *fname, uint8_t *buf, size_t size);

#endif
