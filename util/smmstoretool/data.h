/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SMMSTORETOOL__DATA_H__
#define SMMSTORETOOL__DATA_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum data_type {
	DATA_TYPE_BOOL,
	DATA_TYPE_UINT8,
	DATA_TYPE_UINT16,
	DATA_TYPE_UINT32,
	DATA_TYPE_UINT64,
	DATA_TYPE_ASCII,
	DATA_TYPE_UNICODE,
	DATA_TYPE_FILE,
	DATA_TYPE_RAW,
};

void print_data(const uint8_t data[], size_t data_size, enum data_type type);

void *make_data(const char source[], size_t *data_size, enum data_type type);

bool parse_data_type(const char str[], enum data_type *type);

#endif // SMMSTORETOOL__DATA_H__
