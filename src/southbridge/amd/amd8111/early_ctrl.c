/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 yhlu
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

#include "amd8111.h"
#include <reset.h>

unsigned get_sbdn(unsigned bus)
{
	pci_devfn_t dev;

	/* Find the device.
	 * There can only be one 8111 on a hypertransport chain/bus.
	 */
	dev = pci_locate_device_on_bus(
		PCI_ID(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_8111_PCI),
		bus);

	return (dev>>15) & 0x1f;

}

static void enable_cf9_x(unsigned sbbusn, unsigned sbdn)
{
	pci_devfn_t dev;
	uint8_t byte;

	dev = PCI_DEV(sbbusn, sbdn+1, 3); //ACPI
	/* enable cf9 */
	byte = pci_read_config8(dev, 0x41);
	byte |= (1<<6) | (1<<5);
	pci_write_config8(dev, 0x41, byte);
}

static void enable_cf9(void)
{
	unsigned sblk = get_sblk();
	unsigned sbbusn = get_sbbusn(sblk);
	unsigned sbdn = get_sbdn(sbbusn);

	enable_cf9_x(sbbusn, sbdn);
}

void do_hard_reset(void)
{
	set_bios_reset();
	/* reset */
	enable_cf9();
	outb(0x0e, 0x0cf9); // make sure cf9 is enabled
}

void enable_fid_change_on_sb(unsigned sbbusn, unsigned sbdn)
{
	pci_devfn_t dev;

	dev = PCI_DEV(sbbusn, sbdn+1, 3); // ACPI

	pci_write_config8(dev, 0x74, 4);

	/* set VFSMAF ( VID/FID System Management Action Field) to 2 */
	pci_write_config32(dev, 0x70, 2<<12);

}

static void soft_reset_x(unsigned sbbusn, unsigned sbdn)
{
	pci_devfn_t dev;

	dev = PCI_DEV(sbbusn, sbdn+1, 0); //ISA

	/* Reset */
	set_bios_reset();
	pci_write_config8(dev, 0x47, 1);

}

void do_soft_reset(void)
{

	unsigned sblk = get_sblk();
	unsigned sbbusn = get_sbbusn(sblk);
	unsigned sbdn = get_sbdn(sbbusn);

	return soft_reset_x(sbbusn, sbdn);

}
