/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include "board.h"
#include <gpio.h>
#include <soc/aop_common.h>
#include <soc/qclib_common.h>
#include <soc/shrm.h>

void platform_romstage_main(void)
{
	void (*const fw_init_sequence[])(void) = {
		shrm_fw_load_reset,
		qclib_load_and_run,
		aop_fw_load_reset,
		qclib_rerun,
	};

	/* Executing essential firmware loading */
	for (size_t i = 0; i < ARRAY_SIZE(fw_init_sequence); i++)
		fw_init_sequence[i]();

	/*
	 * Enable this power rail now for FPMCU stability prior to
	 * its reset being deasserted in ramstage. This applies
	 * when MAINBOARD_HAS_FINGERPRINT_VIA_SPI Kconfig is enabled.
	 * Requires >=200ms delay after its pin was driven low in bootblock.
	 */
	if (CONFIG(MAINBOARD_HAS_FINGERPRINT_VIA_SPI))
		gpio_output(GPIO_EN_FP_RAILS, 1);
}
