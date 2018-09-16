/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

// __PRE_RAM__ means: use "unsigned" for device, not a struct.

#include <device/pci_ops.h>
#include <console/console.h>
#include <southbridge/intel/i82801ix/i82801ix.h>
#include <southbridge/intel/common/gpio.h>
#include <northbridge/intel/gm45/gm45.h>
#include <drivers/lenovo/hybrid_graphics/hybrid_graphics.h>
#include "dock.h"

#define LPC_DEV PCI_DEV(0, 0x1f, 0)

static void hybrid_graphics_init(sysinfo_t *sysinfo)
{
	bool peg, igd;

	early_hybrid_graphics(&igd, &peg);

	sysinfo->enable_igd = igd;
	sysinfo->enable_peg = peg;
}

static int dock_err;

void mb_setup_lpc(void)
{
	/* Set up SuperIO LPC forwards */

	/* Configure serial IRQs.*/
	pci_write_config8(LPC_DEV, D31F0_SERIRQ_CNTL, 0xd0);
	/* Map COMa on 0x3f8, COMb on 0x2f8. */
	pci_write_config16(LPC_DEV, D31F0_LPC_IODEC, 0x0010);
	pci_write_config16(LPC_DEV, D31F0_LPC_EN, 0x3f0f);
	pci_write_config32(LPC_DEV, D31F0_GEN1_DEC, 0x7c1601);
	pci_write_config32(LPC_DEV, D31F0_GEN2_DEC, 0xc15e1);
	pci_write_config32(LPC_DEV, D31F0_GEN3_DEC, 0x1c1681);
}

void mb_setup_superio(void)
{
	/* Minimal setup to detect dock */
	dock_err = pc87382_early();
	if (dock_err == 0)
		dock_connect();
}

void get_mb_spd_addrmap(u8 *spd_addrmap)
{
	spd_addrmap[0] = 0x50;
	spd_addrmap[2] = 0x51;
}

void mb_pre_raminit_setup(sysinfo_t *sysinfo)
{
	/* Console is not yet initialized in mb_setup_superio, so we print
	   the dock information here */
	if (dock_err)
		printk(BIOS_ERR, "DOCK: Failed to init pc87382\n");
	else
		dock_info();

	if (CONFIG(BOARD_LENOVO_R500)) {
		int use_integrated = get_gpio(21);
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
