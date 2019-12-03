/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <device/pci_ops.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <southbridge/amd/agesa/hudson/hudson.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct5104d/nct5104d.h>

#define SERIAL_DEV PNP_DEV(0x4E, NCT5104D_SP4)

void board_BeforeAgesa(struct sysinfo *cb)
{
	u32 *addr32;
	u32 t32;

	/* For serial port option, plug-in card on LPC. */
	pci_devfn_t dev = PCI_DEV(0, 0x14, 3);
	pci_write_config32(dev, 0x44, 0xff03ffd5);

	/* In Hudson RRG, PMIOxD2[5:4] is "Drive strength control for
	 *  LpcClk[1:0]".  To be consistent with Parmer, setting to 4mA
	 *  even though the register is not documented in the Kabini BKDG.
	 *  Otherwise the serial output is bad code.
	 */
	outb(0xD2, 0xcd6);
	outb(0x00, 0xcd7);


	/* Enable the AcpiMmio space */
	outb(0x24, 0xcd6);
	outb(0x01, 0xcd7);

	/* Set auxiliary output clock frequency on OSCOUT1 pin to be 25MHz */
	/* Set auxiliary output clock frequency on OSCOUT2 pin to be 48MHz */
	addr32 = (u32 *)0xfed80e28;
	t32 = *addr32;
	t32 &= 0xffc0ffff; // Clr bits [21:19] & [18:16]
	t32 |= 0x00010000; // Set bit 16 for 25MHz
	*addr32 = t32;

	/* Enable Auxiliary OSCOUT1/OSCOUT2 */
	addr32 = (u32 *)0xfed80e40;
	t32 = *addr32;
	t32 &= 0xffffff7b; // clear 2, 7
	*addr32 = t32;

	nct5104d_enable_uartd(SERIAL_DEV);
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

#if 0
	/* Was before copy_and_run. */
	outb(0xEA, 0xCD6);
	outb(0x1, 0xcd7);
#endif
