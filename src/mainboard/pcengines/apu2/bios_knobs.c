/*
 * This file is part of the coreboot project.

 *
 * Copyright (C) 2017 3mdeb
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <console/console.h>
#include <program_loading.h>
#include <cbfs.h>
#include <string.h>
#include <commonlib/cbfs.h>
#include <commonlib/region.h>
#include <commonlib/cbfs_serialized.h>
#include "bios_knobs.h"

#define BOOTORDER_FILE "bootorder"

static char *after_str(const char *s, const char *pattern)
{
    size_t pattern_l = strlen (pattern);
    while ((s = strchr (s, pattern[0])) != NULL) {
        if (strncmp (s, pattern, pattern_l) == 0)
            return (char*)s+pattern_l;
        s++;
    }

	return NULL;
}

#define map_cbfs_file(...) cbfs_boot_map_with_leak (__VA_ARGS__);
#define fail(...) {printk (__VA_ARGS__); return -1;}

static u8 check_knob_value(const char *s)
{
	const char *boot_file = NULL;
	size_t boot_file_len = 0;
	char * token = NULL;

	boot_file = map_cbfs_file (BOOTORDER_FILE, CBFS_TYPE_RAW, &boot_file_len);
	if (boot_file == NULL)
		fail(BIOS_ALERT, "file [%s] not found in CBFS\n", BOOTORDER_FILE);
	if (boot_file_len < 4096)
		fail(BIOS_ALERT, "Missing bootorder data.\n");

	if ((token = after_str (boot_file, s)) != NULL) {
		if (*token == '0') return 0;
		if (*token == '1') return 1;
        else return -2;
	}

	return -1;
}

#define opt_function(NAME, PAR_NAME, DEFAULT) \
    bool NAME(void) \
    { \
        u8 val = check_knob_value (PAR_NAME); \
        if (val == 0) \
            return false; \
        else if (val == 1) \
            return true; \
        else { \
		    printk(BIOS_EMERG, "Missing or invalid " PAR_NAME \
                  " knob, default: " #DEFAULT ".\n"); \
            return DEFAULT; \
        }\
    }

opt_function (check_console, "scon", true)
opt_function (check_uartc, "uartc", false)
opt_function (check_uartd, "uartd", false)
opt_function (check_ehci0, "ehcien", true)
opt_function (check_mpcie2_clk, "mpcie2_clk", false)
