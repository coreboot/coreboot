/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 3mdeb
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

#include <option.h>

#include <cbfs.h>
#include <string.h>
#include <types.h>

#include <commonlib/cbfs.h>
#include <commonlib/region.h>
#include <commonlib/cbfs_serialized.h>
#include <console/console.h>

#define CONFIG_FILE "option_table"

static char *after_str(const char *s, const char *pattern)
{
	size_t pattern_l = strlen(pattern);
	while ((s = strchr(s, pattern[0])) != NULL) {
		if (strncmp(s, pattern, pattern_l) == 0)
			return (char *)s+pattern_l;
		s++;
	}

	return NULL;
}

#define map_cbfs_file(...) cbfs_boot_map_with_leak(__VA_ARGS__)

enum cb_err get_option(void *dest, const char *name)
{
	const char *boot_file = NULL;
	size_t boot_file_len = 0;
	char *token = NULL;

	boot_file = map_cbfs_file(
		CONFIG_FILE, CBFS_TYPE_RAW, &boot_file_len
	);

	if (boot_file == NULL) {
		printk(BIOS_ALERT,
			"file [%s] not found in CBFS\n", CONFIG_FILE
		);
		return CB_CMOS_OPTION_NOT_FOUND;
	}

	token = after_str(boot_file, name);
	if (token != NULL) {
		*((u8 *)dest) = *token - '0';
		return CB_SUCCESS;
	}

	return CB_CMOS_OPTION_NOT_FOUND;
}

