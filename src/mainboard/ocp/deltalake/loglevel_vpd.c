/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <string.h>

#include "vpd.h"

int get_console_loglevel(void)
{
	int log_level = COREBOOT_LOG_LEVEL_DEFAULT;

	if (vpd_get_int(COREBOOT_LOG_LEVEL, VPD_RW_THEN_RO, &log_level)) {
		if (log_level < 0 || log_level >= BIOS_NEVER)
			log_level = COREBOOT_LOG_LEVEL_DEFAULT;
	}
	return log_level;
}
