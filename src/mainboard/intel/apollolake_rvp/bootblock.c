/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <bootblock_common.h>
#include <console/console.h>
#include <soc/gpio.h>
#include <soc/gpio_defs.h>
#include <soc/uart.h>

static struct pad_config aplk_rvp_gpios[] = {
	PAD_CFG_NF(GPIO_46, NATIVE, DEEP, NF1),		/* UART2 RX*/
	PAD_CFG_NF(GPIO_47, NATIVE, DEEP, NF1)		/* UART2 TX*/
};

void bootblock_mainboard_early_init(void)
{
	if (IS_ENABLED(CONFIG_BOOTBLOCK_CONSOLE)) {
		gpio_configure_pads(aplk_rvp_gpios, ARRAY_SIZE(aplk_rvp_gpios));
		lpss_console_uart_init();
	}
}
