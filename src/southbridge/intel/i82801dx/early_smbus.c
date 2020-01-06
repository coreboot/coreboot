/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Ronald G. Minnich
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

#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <device/smbus_host.h>
#include "i82801dx.h"

void i82801dx_early_init(void)
{
	enable_smbus();
}

uintptr_t smbus_base(void)
{
	return SMBUS_IO_BASE;
}

int smbus_enable_iobar(uintptr_t base)
{
	pci_devfn_t dev = PCI_DEV(0x0, 0x1f, 0x3);

	/* set smbus iobase */
	pci_write_config32(dev, 0x20, base | 1);
	/* Set smbus enable */
	pci_write_config8(dev, 0x40, 0x01);
	/* Set smbus iospace enable */
	pci_write_config16(dev, 0x4, 0x01);

	return 0;
}

int smbus_read_byte(unsigned int device, unsigned int address)
{
	return do_smbus_read_byte(SMBUS_IO_BASE, device, address);
}
