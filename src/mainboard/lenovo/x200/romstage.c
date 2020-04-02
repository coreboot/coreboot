/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <southbridge/intel/common/gpio.h>
#include <northbridge/intel/gm45/gm45.h>

void get_mb_spd_addrmap(u8 *spd_addrmap)
{
	spd_addrmap[0] = 0x50;
	spd_addrmap[2] = 0x51;
}

void mb_post_raminit_setup(void)
{
	/* FIXME: make a proper SMBUS mux support. */
	/* Set the SMBUS mux to the eeprom */
	set_gpio(42, GPIO_LEVEL_LOW);
}
