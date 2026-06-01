/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8613e/it8613e.h>

#define GPIO_DEV PNP_DEV(0x2e, IT8613E_GPIO)
#define UART_DEV PNP_DEV(0x2e, IT8613E_SP1)

void bootblock_mainboard_early_init(void)
{
	/* Set up Super I/O GPIOs, values are dumped from vendor firmware */
	ite_reg_write(GPIO_DEV, 0x26, 0xfb);
	ite_reg_write(GPIO_DEV, 0x29, 0x01);
	ite_reg_write(GPIO_DEV, 0x2c, 0x41);
	ite_reg_write(GPIO_DEV, 0x2d, 0x02);
	ite_reg_write(GPIO_DEV, 0xbc, 0xc0);
	ite_reg_write(GPIO_DEV, 0xbd, 0x03);
	ite_reg_write(GPIO_DEV, 0xc1, 0x0a);
	ite_reg_write(GPIO_DEV, 0xc8, 0x00);
	ite_reg_write(GPIO_DEV, 0xc9, 0x0a);
	ite_reg_write(GPIO_DEV, 0xda, 0xb0);
	ite_reg_write(GPIO_DEV, 0xdb, 0x44);

	ite_delay_pwrgd3(GPIO_DEV);

	ite_enable_serial(UART_DEV, CONFIG_TTYS0_BASE);
}

void bootblock_mainboard_init(void)
{
}
