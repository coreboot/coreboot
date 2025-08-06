/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/gpio.h>
#include <device/pnp_ops.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8659e/it8659e.h>
#include "gpio.h"

#define UART_DEV PNP_DEV(0x4e, IT8659E_SP1)
#define GPIO_DEV PNP_DEV(0x4e, IT8659E_GPIO)

void bootblock_mainboard_early_init(void)
{
	// Early eSPI and LPSS UART initialization
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));

	ite_reg_write(GPIO_DEV, 0x26, 0xc0);
	ite_reg_write(GPIO_DEV, 0x29, 0x24);
	ite_reg_write(GPIO_DEV, 0x2c, 0x88);

	// COM1 (CP2105 Port B)
	ite_enable_serial(UART_DEV, CONFIG_TTYS0_BASE);
}
