/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "data.h"

#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void print_data(const uint8_t data[], size_t data_size, enum data_type type)
{
	if (data_size == 0)
		return;

	switch (type) {
	case DATA_TYPE_BOOL:
		bool value = false;
		for (size_t i = 0; i < data_size; ++i) {
			if (data[i] != 0) {
				value = true;
				break;
			}
		}
		printf("%s\n", value ? "true" : "false");
		break;
	case DATA_TYPE_UINT8:
		if (data_size != 1) {
			fprintf(stderr,
				"warning: expected size of 1, got %zu\n",
				data_size);
		}

		if (data_size >= 1)
			printf("%u\n", *(uint8_t *)data);
		break;
	case DATA_TYPE_UINT16:
		if (data_size != 2) {
			fprintf(stderr,
				"warning: expected size of 2, got %zu\n",
				data_size);
		}

		if (data_size >= 2)
			printf("%u\n", *(uint16_t *)data);
		break;
	case DATA_TYPE_UINT32:
		if (data_size != 4) {
			fprintf(stderr,
				"warning: expected size of 4, got %zu\n",
				data_size);
		}

		if (data_size >= 4)
			printf("%u\n", *(uint32_t *)data);
		break;
	case DATA_TYPE_UINT64:
		if (data_size != 8) {
			fprintf(stderr,
				"warning: expected size of 8, got %zu\n",
				data_size);
		}

		if (data_size >= 8)
			printf("%llu\n", (unsigned long long)*(uint64_t *)data);
		break;
	case DATA_TYPE_ASCII:
		for (size_t i = 0; i < data_size; ++i) {
			char c = data[i];
			if (isprint(c))
				printf("%c", c);
		}
		printf("\n");
		break;
	case DATA_TYPE_UNICODE:
		char *chars = to_chars((const CHAR16 *)data, data_size);
		printf("%s\n", chars);
		free(chars);
		break;
	case DATA_TYPE_FILE:
		fprintf(stderr, "File data type is input only\n");
		break;
	case DATA_TYPE_RAW:
		fwrite(data, 1, data_size, stdout);
		break;
	}
}

static uint64_t parse_uint(const char source[],
			   const char type[],
			   unsigned long long max,
			   bool *failed)
{
	char *end;
	unsigned long long uint = strtoull(source, &end, /*base=*/0);
	if (*end != '\0') {
		fprintf(stderr, "Trailing characters in \"%s\": %s\n",
			source, end);
		*failed = true;
		return 0;
	}
	if (uint > max) {
		fprintf(stderr, "Invalid %s value: %llu\n", type, uint);
		*failed = true;
		return 0;
	}

	*failed = false;
	return uint;
}

void *make_data(const char source[], size_t *data_size, enum data_type type)
{
	switch (type) {
	void *data;
	bool boolean;
	uint64_t uint;
	struct mem_range_t file;
	bool failed;

	case DATA_TYPE_BOOL:
		if (str_eq(source, "true")) {
			boolean = true;
		} else if (str_eq(source, "false")) {
			boolean = false;
		} else {
			fprintf(stderr, "Invalid boolean value: \"%s\"\n",
				source);
			return NULL;
		}

		*data_size = 1;
		data = xmalloc(*data_size);
		*(uint8_t *)data = boolean;
		return data;
	case DATA_TYPE_UINT8:
		uint = parse_uint(source, "uint8", UINT8_MAX, &failed);
		if (failed)
			return NULL;

		*data_size = 1;
		data = xmalloc(*data_size);
		*(uint8_t *)data = uint;
		return data;
	case DATA_TYPE_UINT16:
		uint = parse_uint(source, "uint16", UINT16_MAX, &failed);
		if (failed)
			return NULL;

		*data_size = 2;
		data = xmalloc(*data_size);
		*(uint16_t *)data = uint;
		return data;
	case DATA_TYPE_UINT32:
		uint = parse_uint(source, "uint32", UINT32_MAX, &failed);
		if (failed)
			return NULL;

		*data_size = 4;
		data = xmalloc(*data_size);
		*(uint32_t *)data = uint;
		return data;
	case DATA_TYPE_UINT64:
		uint = parse_uint(source, "uint64", UINT64_MAX, &failed);
		if (failed)
			return NULL;

		*data_size = 8;
		data = xmalloc(*data_size);
		*(uint64_t *)data = uint;
		return data;
	case DATA_TYPE_ASCII:
		*data_size = strlen(source) + 1;
		return strdup(source);
	case DATA_TYPE_UNICODE:
		return to_uchars(source, data_size);
	case DATA_TYPE_FILE:
		file = map_file(source, /*rw=*/false);
		if (file.start == NULL || file.length == 0)
			return NULL;

		*data_size = file.length;
		data = xmalloc(*data_size);
		memcpy(data, file.start, *data_size);
		unmap_file(file);
		return data;
	case DATA_TYPE_RAW:
		fprintf(stderr, "Raw data type is output only\n");
		return NULL;
	}

	return NULL;
}

bool parse_data_type(const char str[], enum data_type *type)
{
	if (str_eq(str, "bool"))
		*type = DATA_TYPE_BOOL;
	else if (str_eq(str, "uint8"))
		*type = DATA_TYPE_UINT8;
	else if (str_eq(str, "uint16"))
		*type = DATA_TYPE_UINT16;
	else if (str_eq(str, "uint32"))
		*type = DATA_TYPE_UINT32;
	else if (str_eq(str, "uint64"))
		*type = DATA_TYPE_UINT64;
	else if (str_eq(str, "ascii"))
		*type = DATA_TYPE_ASCII;
	else if (str_eq(str, "unicode"))
		*type = DATA_TYPE_UNICODE;
	else if (str_eq(str, "file"))
		*type = DATA_TYPE_FILE;
	else if (str_eq(str, "raw"))
		*type = DATA_TYPE_RAW;
	else
		return false;

	return true;
}
