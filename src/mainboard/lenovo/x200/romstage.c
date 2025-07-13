/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <northbridge/intel/gm45/gm45.h>

void get_mb_spd_addrmap(u8 spd_addrmap[4])
{
	spd_addrmap[0] = 0x50;
	spd_addrmap[2] = 0x51;
}

void mb_post_raminit_setup(void)
{
	/* FIXME: make a proper SMBUS mux support. */
	/* Set the SMBUS mux to the eeprom */
	gpio_set(42, GPIO_LEVEL_LOW);
}
