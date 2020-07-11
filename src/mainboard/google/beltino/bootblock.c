/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/lynxpoint/pch.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8772f/it8772f.h>
#include "onboard.h"

void mainboard_config_superio(void)
{
	/* Early SuperIO setup */
	ite_kill_watchdog(IT8772F_GPIO_DEV);
	it8772f_ac_resume_southbridge(IT8772F_SUPERIO_DEV);
	ite_enable_serial(IT8772F_SERIAL_DEV, CONFIG_TTYS0_BASE);

	/* Turn on Power LED */
	set_power_led(LED_ON);
}
