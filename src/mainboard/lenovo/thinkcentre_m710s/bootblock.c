/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8629e/it8629e.h>

#define SERIAL_DEV PNP_DEV(0x2e, IT8629E_SP1)
#define GPIO_DEV   PNP_DEV(0x2e, IT8629E_GPIO)

void bootblock_mainboard_early_init(void)
{
	ite_reg_write(GPIO_DEV, 0x23, 0x49);
	ite_reg_write(GPIO_DEV, 0x71, 0x09);
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
