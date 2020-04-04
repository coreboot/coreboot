/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <soc/mt8183.h>
#include <soc/wdt.h>
#include <soc/gpio.h>

void mt8183_early_init(void)
{
	mtk_wdt_init();
	gpio_set_i2c_eh_rsel();
}
