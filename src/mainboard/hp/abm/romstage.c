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

#include <amdblocks/acpimmio.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <southbridge/amd/agesa/hudson/hudson.h>
#include <amdblocks/acpimmio.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct5104d/nct5104d.h>

#define SERIAL_DEV PNP_DEV(0x4E, NCT5104D_SP4)

void board_BeforeAgesa(struct sysinfo *cb)
{
	u32 t32;

	/* For serial port option, plug-in card on LPC. */
	pci_devfn_t dev = PCI_DEV(0, 0x14, 3);
	pci_write_config32(dev, 0x44, 0xff03ffd5);

	/* In Hudson RRG, PMIOxD2[5:4] is "Drive strength control for
	 *  LpcClk[1:0]".  To be consistent with Parmer, setting to 4mA
	 *  even though the register is not documented in the Kabini BKDG.
	 *  Otherwise the serial output is bad code.
	 */
	pm_io_write8(0xd2, 0);

	/* Enable the AcpiMmio space */
	pm_io_write8(0x24, 1);

	/* Set auxiliary output clock frequency on OSCOUT1 pin to be 25MHz */
	/* Set auxiliary output clock frequency on OSCOUT2 pin to be 48MHz */
	t32 = misc_read32(0x28);
	t32 &= 0xffc0ffff; // Clr bits [21:19] & [18:16]
	t32 |= 0x00010000; // Set bit 16 for 25MHz
	misc_write(0x28, t32);

	/* Enable Auxiliary OSCOUT1/OSCOUT2 */
	t32 = misc_write32(0x40, misc_read32(0x40) & 0xffffff7b);

	nct5104d_enable_uartd(SERIAL_DEV);
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
