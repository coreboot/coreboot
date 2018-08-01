/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Facebook Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __TCPA_LOG_SERIALIZED_H__
#define __TCPA_LOG_SERIALIZED_H__

#include <compiler.h>
#include <stdint.h>

#define MAX_TCPA_LOG_ENTRIES 50
#define TCPA_LOG_STRING_LENGTH 512
#define TCPA_FORMAT_HASH_LENGTH 128
#define TCPA_DIGEST_MAX_LENGTH 64
#define TCPA_PCR_HASH_NAME 256

struct tcpa_entry {
	uint32_t pcr;
	uint8_t digest[TCPA_DIGEST_MAX_LENGTH];
	uint32_t digest_length;
	char name[TCPA_PCR_HASH_NAME];
} __packed;

struct tcpa_table {
	uint16_t max_entries;
	uint16_t num_entries;
	struct tcpa_entry entries[0]; /* Variable number of entries */
} __packed;

#endif
