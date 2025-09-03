/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include "board.h"
#include <gpio.h>
#include <soc/aop_common.h>
#include <soc/qclib_common.h>
#include <soc/shrm.h>
#include <soc/watchdog.h>

void platform_romstage_main(void)
{
	/* Watchdog must be checked first to avoid erasing watchdog info later. */
	check_wdog();

	shrm_fw_load_reset();

	/* QCLib: DDR init & train */
	qclib_load_and_run();

	enable_slow_battery_charging();

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
