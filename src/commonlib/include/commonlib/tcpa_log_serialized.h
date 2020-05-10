/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __TCPA_LOG_SERIALIZED_H__
#define __TCPA_LOG_SERIALIZED_H__

#include <stdint.h>

#define MAX_TCPA_LOG_ENTRIES 50
#define TCPA_DIGEST_MAX_LENGTH 64
#define TCPA_PCR_HASH_NAME 50
#define TCPA_PCR_HASH_LEN 10
/* Assumption of 2K TCPA log size reserved for CAR/SRAM */
#define MAX_PRERAM_TCPA_LOG_ENTRIES 15

struct tcpa_entry {
	uint32_t pcr;
	char digest_type[TCPA_PCR_HASH_LEN];
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
