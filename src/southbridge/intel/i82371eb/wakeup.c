/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdint.h>
#include <acpi/acpi.h>
#include <arch/io.h>
#include <console/console.h>
#include "i82371eb.h"

/*
 * Intel 82371EB (PIIX4E) datasheet, section 7.2.3, page 142
 *
 * 0: soft off/suspend to disk					S5
 * 1: suspend to ram						S3
 * 2: powered on suspend, context lost				S2
 *    Note: 'context lost' means the CPU restarts at the reset
 *          vector
 * 3: powered on suspend, CPU context lost			S1
 *    Note: Looks like 'CPU context lost' does _not_ mean the
 *          CPU restarts at the reset vector. Most likely only
 *          caches are lost, so both 0x3 and 0x4 map to acpi S1
 * 4: powered on suspend, context maintained			S1
 * 5: working (clock control)					S0
 * 6: reserved
 * 7: reserved
 */
static const u8 acpi_sus_to_slp_typ[8] = {
	5, 3, 2, 1, 1, 0, 0, 0
};

int acpi_get_sleep_type(void)
{
	u16 reg, result;

	reg = inw(DEFAULT_PMBASE + PMCNTRL);
	result = acpi_sus_to_slp_typ[(reg >> 10) & 7];

	printk(BIOS_DEBUG, "Wakeup from ACPI sleep type S%d (PMCNTRL=%04x)\n", result, reg);

	return result;
}
