/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/pmlib.h>
#include <console/console.h>
#include <types.h>

/* This register is a bit of an odd one. The configuration gets written into the lower nibble,
   but ends up being copied to the upper nibble which gets initialized by this. */
#define PM_RTC_SHADOW_REG	0x5b
#define   PWR_PWRSTATE		BIT(2) /* power state bit; needs to be written as 1 */
#define   PWR_FAIL_OFF		0x0 /* Always power off after power resumes */
#define   PWR_FAIL_ON		0x1 /* Always power on after power resumes */
#define   PWR_FAIL_PREV		0x3 /* Use previous setting after power resumes */

void pm_set_power_failure_state(void)
{
	uint8_t pwr_fail = PWR_PWRSTATE;

	switch (CONFIG_MAINBOARD_POWER_FAILURE_STATE) {
	case MAINBOARD_POWER_STATE_OFF:
		printk(BIOS_INFO, "Set power off after power failure.\n");
		pwr_fail |= PWR_FAIL_OFF;
		break;
	case MAINBOARD_POWER_STATE_ON:
		printk(BIOS_INFO, "Set power on after power failure.\n");
		pwr_fail |= PWR_FAIL_ON;
		break;
	case MAINBOARD_POWER_STATE_PREVIOUS:
		printk(BIOS_INFO, "Keep power state after power failure.\n");
		pwr_fail |= PWR_FAIL_PREV;
		break;
	default:
		printk(BIOS_WARNING, "WARNING: Unknown power-failure state: %d\n",
						CONFIG_MAINBOARD_POWER_FAILURE_STATE);
		pwr_fail |= PWR_FAIL_OFF;
		break;
	}

	pm_io_write8(PM_RTC_SHADOW_REG, pwr_fail);
}
