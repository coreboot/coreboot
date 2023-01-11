/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <soc/mt8183.h>
#include <soc/wdt.h>

void mt8183_early_init(void)
{
	mtk_wdt_init();
	gpio_set_i2c_eh_rsel();
}
