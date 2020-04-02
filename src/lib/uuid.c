/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stdint.h>
#include <lib.h>
#include <uuid.h>

int parse_uuid(uint8_t *const uuid, const char *const uuid_str)
{
	const uint8_t order[] = { 3, 2, 1, 0, 5, 4, 7, 6, 8, 9, 10, 11, 12, 13, 14, 15 };
	uint8_t uuid_binstr[UUID_LEN];
	unsigned int i;

	if (strlen(uuid_str) != UUID_STRLEN)
		return -1;
	if (uuid_str[8] != '-' || uuid_str[13] != '-' ||
	    uuid_str[18] != '-' || uuid_str[23] != '-')
		return -1;
	if (hexstrtobin(uuid_str, uuid_binstr, UUID_LEN) != UUID_LEN)
		return -1;
	for (i = 0; i < UUID_LEN; ++i)
		uuid[i] = uuid_binstr[order[i]];

	return 0;
}
