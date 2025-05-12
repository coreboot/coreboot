/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <gpio.h>
#include "board.h"

void platform_romstage_main(void)
{
	/* Placeholder */

	/*
	 * Enable this power rail now for FPMCU stability prior to
	 * its reset being deasserted in ramstage. This applies
	 * when MAINBOARD_HAS_FINGERPRINT_VIA_SPI Kconfig is enabled.
	 * Requires >=200ms delay after its pin was driven low in bootblock.
	 */
	if (CONFIG(MAINBOARD_HAS_FINGERPRINT_VIA_SPI))
		gpio_output(GPIO_EN_FP_RAILS, 1);
}
