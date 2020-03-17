/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootblock_common.h>
#include <superio/ite/it8718f/it8718f.h>
#include <superio/ite/common/ite.h>
#include <northbridge/intel/i945/i945.h>
#include <southbridge/intel/i82801gx/i82801gx.h>

#define SERIAL_DEV PNP_DEV(0x2e, IT8718F_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8718F_GPIO)
#define EC_DEV PNP_DEV(0x2e, IT8718F_EC)

void bootblock_mainboard_early_init(void)
{
	/* Set default GPIOs on superio */
	ite_reg_write(GPIO_DEV, 0x25, 0x40);
	ite_reg_write(GPIO_DEV, 0x26, 0x3f);
	ite_reg_write(GPIO_DEV, 0x28, 0x41);
	ite_reg_write(GPIO_DEV, 0x29, 0x88);
	ite_reg_write(GPIO_DEV, 0x2c, 0x1c);
	ite_reg_write(GPIO_DEV, 0x62, 0x08);
	ite_kill_watchdog(GPIO_DEV);
	ite_reg_write(GPIO_DEV, 0xb1, 0x01);
	ite_reg_write(GPIO_DEV, 0xb8, 0x80);
	ite_reg_write(GPIO_DEV, 0xbb, 0x40);
	ite_reg_write(GPIO_DEV, 0xc0, 0x00);
	ite_reg_write(GPIO_DEV, 0xc3, 0x00);
	ite_reg_write(GPIO_DEV, 0xc8, 0x00);
	ite_reg_write(GPIO_DEV, 0xcb, 0x00);
	ite_reg_write(GPIO_DEV, 0xf6, 0x26);
	ite_reg_write(GPIO_DEV, 0xfc, 0x01);

	ite_reg_write(EC_DEV, 0x70, 0x00); // Don't use IRQ9
	ite_reg_write(EC_DEV, 0x30, 0xff); // Enable

	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	/* Disable SIO reboot */
	ite_reg_write(GPIO_DEV, 0xEF, 0x7E);
}

void mainboard_late_rcba_config(void)
{
	/* Enable only PCIe Root Port Clock Gate */
	RCBA32(CG) = 0x00000001;
}
