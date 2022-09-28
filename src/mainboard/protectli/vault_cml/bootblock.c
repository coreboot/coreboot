/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/gpio.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8786e/it8786e.h>
#include "gpio.h"

#define UART_DEV PNP_DEV(0x2e, IT8786E_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8786E_GPIO)

void bootblock_mainboard_early_init(void)
{
	/* CLKIN freq 24MHz, Ext CLKIN for Watchdog, Internal VCC_OK */
	ite_reg_write(GPIO_DEV, 0x23, 0x49);
	/* Set pin native functions */
	ite_reg_write(GPIO_DEV, 0x26, 0x00);
	/* Set GPIOS exposed on pin header as GPIO functions  */
	ite_reg_write(GPIO_DEV, 0x29, 0xc0);
	/* External CLKIN PCICLK */
	ite_reg_write(GPIO_DEV, 0x71, 0x08);
	/* Enable 3VSB during Suspend-to-RAM */
	ite_enable_3vsbsw(GPIO_DEV);
	/* Delay PWROK2 after 3VSBSW# during resume from Suspend-to-RAM */
	ite_delay_pwrgd3(GPIO_DEV);
	ite_kill_watchdog(GPIO_DEV);
	ite_enable_serial(UART_DEV, CONFIG_TTYS0_BASE);
}

void bootblock_mainboard_init(void)
{
	const struct pad_config *pads;
	size_t num;

	pads = board_gpio_table(&num);
	gpio_configure_pads(pads, num);
}
