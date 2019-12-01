/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 BAP - Bruhnspace Advanced Projects
 * (Written by Fabian Kunkel <fabi@adv.bruhnspace.com> for BAP)
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
#include <amdblocks/acpimmio.h>
#include <device/pci_ops.h>
#include <southbridge/amd/agesa/hudson/hudson.h>

#include <northbridge/amd/agesa/state_machine.h>
#include <superio/fintek/common/fintek.h>
#include <superio/fintek/f81866d/f81866d.h>


#define SERIAL_DEV1 PNP_DEV(0x4e, F81866D_SP1)

void board_BeforeAgesa(struct sysinfo *cb)
{
	/* Disable PCI-PCI bridge and release GPIO32/33 for other uses. */
	pm_io_write(0xea, 1);

	/* Set LPC decode enables. */
	pci_devfn_t dev = PCI_DEV(0, 0x14, 3);
	pci_write_config32(dev, 0x44, 0xff03ffd5);

	fintek_enable_serial(SERIAL_DEV1, CONFIG_TTYS0_BASE);
}
