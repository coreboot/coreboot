/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <smbios.h>
#include <stdio.h>
#include <string.h>

const char *smbios_mainboard_version(void)
{
	static char str[8];

	snprintf(str, sizeof(str), "rev%d", board_id());

	return str;
}
