/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <drivers/lenovo/hybrid_graphics/hybrid_graphics.h>
#include <gpio.h>
#include <northbridge/intel/gm45/gm45.h>
#include <southbridge/intel/common/gpio.h>

static void hybrid_graphics_init(sysinfo_t *sysinfo)
{
	bool peg, igd;

	early_hybrid_graphics(&igd, &peg);

	sysinfo->enable_igd = igd;
	sysinfo->enable_peg = peg;
}

void get_mb_spd_addrmap(u8 spd_addrmap[4])
{
	spd_addrmap[0] = 0x50;
	spd_addrmap[2] = 0x51;
}

void mb_pre_raminit_setup(sysinfo_t *sysinfo)
{
	if (CONFIG(BOARD_LENOVO_R500)) {
		int use_integrated = gpio_get(21);
		printk(BIOS_DEBUG, "R500 variant found with an %s GPU\n",
		       use_integrated ? "integrated" : "discrete");
		if (use_integrated) {
			sysinfo->enable_igd = 1;
			sysinfo->enable_peg = 0;
		} else {
			sysinfo->enable_igd = 0;
			sysinfo->enable_peg = 1;
		}
	} else {
		hybrid_graphics_init(sysinfo);
	}
}

void mb_post_raminit_setup(void)
{
	/* FIXME: make a proper SMBUS mux support. */
	/* Set the SMBUS mux to the eeprom */
	set_gpio(42, GPIO_LEVEL_LOW);
}
