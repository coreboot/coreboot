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
#include <amdblocks/acpimmio.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <amdblocks/acpimmio.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <southbridge/amd/agesa/hudson/hudson.h>
#include <superio/smsc/lpc47n217/lpc47n217.h>

#define SERIAL_DEV PNP_DEV(0x2e, LPC47N217_SP1)

void board_BeforeAgesa(struct sysinfo *cb)
{
	u8 byte;
	pci_devfn_t dev;

	/* Set LPC decode enables. */
	dev = PCI_DEV(0, 0x14, 3);

	byte = pci_read_config8(dev, 0x48);
	byte |= 3;		/* 2e, 2f */
	pci_write_config8(dev, 0x48, byte);

	post_code(0x30);
	/* For serial port. */
	pci_write_config32(dev, 0x44, 0xff03ffd5);
	byte = pci_read_config8(dev, 0x48);
	byte |= 3;		/* 2e, 2f */
	pci_write_config8(dev, 0x48, byte);

	post_code(0x31);
	lpc47n217_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	pm_io_write8(0x24, 1);
	pm_io_write8(0xea, 1);
	gpio_100_write8(0x1, 0x98);
}
