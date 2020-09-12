/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <string.h>

#include "vpd.h"

int get_console_loglevel(void)
{
	int log_level = COREBOOT_LOG_LEVEL_DEFAULT;
	char val_str[VPD_LEN];

	if (vpd_gets(COREBOOT_LOG_LEVEL, val_str, VPD_LEN, VPD_RW_THEN_RO)) {
		log_level = (int)atol(val_str);
		if (log_level < 0 || log_level >= BIOS_NEVER)
			log_level = COREBOOT_LOG_LEVEL_DEFAULT;
	}
	return log_level;
}
