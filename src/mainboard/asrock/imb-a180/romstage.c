/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/stages.h>
#include <device/pnp_def.h>
#include <console/console.h>
#include <commonlib/loglevel.h>

#include <northbridge/amd/agesa/state_machine.h>
#include <southbridge/amd/agesa/hudson/hudson.h>

#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627uhg/w83627uhg.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83627UHG_SP1)

void board_BeforeAgesa(struct sysinfo *cb)
{
	volatile u32 *addr32;
	u32 t32;

	/* Set LPC decode enables. */
	pci_devfn_t dev = PCI_DEV(0, 0x14, 3);
	pci_write_config32(dev, 0x44, 0xff03ffd5);

	hudson_lpc_port80();

	/* Enable the AcpiMmio space */
	outb(0x24, 0xcd6);
	outb(0x1, 0xcd7);

	/* Disable PCI-PCI bridge and release GPIO32/33 for other uses. */
	outb(0xea, 0xcd6);
	outb(0x1, 0xcd7);

	/* Set auxiliary output clock frequency on OSCOUT1 pin to be 48MHz */
	addr32 = (u32 *)0xfed80e28;
	t32 = *addr32;
	t32 &= 0xfff8ffff;
	*addr32 = t32;

	/* Enable Auxiliary Clock1, disable FCH 14 MHz OscClk */
	addr32 = (u32 *)0xfed80e40;
	t32 = *addr32;
	t32 &= 0xffffbffb;
	*addr32 = t32;

	/* w83627uhg has a default clk of 48MHz, p.9 of data-sheet */
	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
