/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include "board.h"
#include <cbmem.h>
#include <commonlib/bsd/cbmem_id.h>
#include <commonlib/coreboot_tables.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <soc/aop_common.h>
#include <soc/qclib_common.h>
#include <soc/shrm.h>
#include <soc/watchdog.h>

static enum boot_mode_t boot_mode = LB_BOOT_MODE_NORMAL;

static void set_boot_mode(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;

	if (is_off_mode())
		boot_mode = LB_BOOT_MODE_OFFMODE_CHARGING;
	else if (google_chromeec_is_below_critical_threshold())
		boot_mode = LB_BOOT_MODE_LOW_BATTERY;
}

void platform_romstage_main(void)
{
	/* Watchdog must be checked first to avoid erasing watchdog info later. */
	check_wdog();

	shrm_fw_load_reset();

	/* QCLib: DDR init & train */
	qclib_load_and_run();

	/* Underlying PMIC registers are accessible only at this point */
	set_boot_mode();

	aop_fw_load_reset();

	qclib_rerun();

	/*
	 * Enable this power rail now for FPMCU stability prior to
	 * its reset being deasserted in ramstage. This applies
	 * when MAINBOARD_HAS_FINGERPRINT_VIA_SPI Kconfig is enabled.
	 * Requires >=200ms delay after its pin was driven low in bootblock.
	 */
	if (CONFIG(MAINBOARD_HAS_FINGERPRINT_VIA_SPI))
		gpio_output(GPIO_EN_FP_RAILS, 1);
}

void platform_romstage_postram(void)
{
	enum boot_mode_t *boot_mode_ptr = cbmem_add(CBMEM_ID_BOOT_MODE, sizeof(*boot_mode_ptr));
	if (boot_mode_ptr) {
		*boot_mode_ptr = boot_mode;
		printk(BIOS_INFO, "Boot mode is %d\n", *boot_mode_ptr);
	}
}
