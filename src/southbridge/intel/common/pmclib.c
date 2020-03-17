/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <arch/acpi.h>
#include <console/console.h>

#include "pmclib.h"
#include "pmbase.h"
#include "pmutil.h"

int southbridge_detect_s3_resume(void)
{
	u32 pm1_cnt;
	u16 pm1_sts;
	int is_s3 = 0;

	/* Check PM1_STS[15] to see if we are waking from Sx */
	pm1_sts = read_pmbase16(PM1_STS);
	if (pm1_sts & WAK_STS) {
		/* Read PM1_CNT[12:10] to determine which Sx state */
		pm1_cnt = read_pmbase32(PM1_CNT);
		if (((pm1_cnt >> 10) & 7) == SLP_TYP_S3) {
			/* Clear SLP_TYPE. */
			write_pmbase32(PM1_CNT, pm1_cnt & ~(7 << 10));
			is_s3 = 1;
		}
	}
	if (is_s3) {
		if (!acpi_s3_resume_allowed()) {
			printk(BIOS_DEBUG, "SB: Resume from S3 detected, but disabled.\n");
			return 0;
		}

		printk(BIOS_DEBUG, "SB: Resume from S3 detected.\n");
	}

	return is_s3;
}
