/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <northbridge/intel/x4x/x4x.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8720f/it8720f.h>

#define GPIO_DEV PNP_DEV(0x2e, IT8720F_GPIO)

void bootblock_mainboard_early_init(void)
{
	/* Set up GPIOs on Super I/O. */
	ite_reg_write(GPIO_DEV, 0x25, 0x00); // GPIO set 1
	ite_reg_write(GPIO_DEV, 0x26, 0x0c); // GPIO set 2
	ite_reg_write(GPIO_DEV, 0x27, 0x70); // GPIO set 3
	ite_reg_write(GPIO_DEV, 0x28, 0x40); // GPIO set 4
	ite_reg_write(GPIO_DEV, 0x29, 0x00); // GPIO set 5

	/* Enable 3VSB during Suspend-to-RAM */
	ite_enable_3vsbsw(GPIO_DEV);

	/* Delay PWROK2 after 3VSBSW# during resume from Suspend-to-RAM */
	ite_delay_pwrgd3(GPIO_DEV);
}

void mb_get_spd_map(u8 spd_map[4])
{
	spd_map[0] = 0x50;
	spd_map[1] = 0x51;
	spd_map[2] = 0x52;
	spd_map[3] = 0x53;
}
