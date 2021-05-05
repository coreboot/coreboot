/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <stdint.h>
#include <acpi/acpi.h>
#include <console/console.h>

#include "pmclib.h"
#include "pmbase.h"
#include "pmutil.h"

static void clear_power_state(void)
{
	uint32_t pm1_cnt = read_pmbase32(PM1_CNT);
	write_pmbase32(PM1_CNT, pm1_cnt & ~SLP_TYP);
}

int southbridge_detect_s3_resume(void)
{
	if (platform_is_resuming()) {
		clear_power_state();
		if (!acpi_s3_resume_allowed()) {
			printk(BIOS_DEBUG, "SB: Resume from S3 detected, but disabled.\n");
			return 0;
		}

		printk(BIOS_DEBUG, "SB: Resume from S3 detected.\n");
		return 1;
	}

	return 0;
}
