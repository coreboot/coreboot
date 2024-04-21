/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8613e/it8613e.h>

#define GPIO_DEV PNP_DEV(0x2e, IT8613E_GPIO)

void bootblock_mainboard_early_init(void)
{
	/* Set up GPIOs on Super I/O. */
	ite_reg_write(GPIO_DEV, 0x25, 0x01); // Enable Pin GP10
	ite_reg_write(GPIO_DEV, 0x27, 0x02); // Enable Pin GP31
	ite_reg_write(GPIO_DEV, 0x28, 0x01); // Enable Pin GP40
	ite_reg_write(GPIO_DEV, 0x29, 0x01); // Enable Pin GP50
	ite_reg_write(GPIO_DEV, 0x2c, 0x41); // Internal Voltage Divider for ACC3
	ite_reg_write(GPIO_DEV, 0xbc, 0xc0); // GP56, GP57 Internal pullup
	ite_reg_write(GPIO_DEV, 0xbd, 0x03); // GP60, GP61 Internal pullup
	ite_reg_write(GPIO_DEV, 0xc3, 0x41); // GP40, GP46 Simple I/O function
	ite_set_3vsbsw(GPIO_DEV, true);
	ite_delay_pwrgd3(GPIO_DEV);
}

void bootblock_mainboard_init(void)
{
}
