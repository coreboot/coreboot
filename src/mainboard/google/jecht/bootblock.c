/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8772f/it8772f.h>
#include "onboard.h"

void bootblock_mainboard_early_init(void)
{
	/* Early SuperIO setup */
	it8772f_ac_resume_southbridge(IT8772F_SUPERIO_DEV);
	ite_kill_watchdog(IT8772F_GPIO_DEV);
	ite_enable_serial(IT8772F_SERIAL_DEV, CONFIG_TTYS0_BASE);

	/* Turn On Power LED */
	set_power_led(LED_ON);
}
