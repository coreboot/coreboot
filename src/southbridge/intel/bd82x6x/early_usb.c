/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Vladimir Serbinenko
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

#include <arch/io.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include <northbridge/intel/sandybridge/sandybridge.h> /* For DEFAULT_RCBABASE.  */
#include "pch.h"

void
early_usb_init (const struct southbridge_usb_port *portmap)
{
	u32 reg32;
	const u32 rcba_dump[8] = {
		/* 3560 */ 0x024c8001, 0x000024a3, 0x00040002, 0x01000050,
		/* 3570 */ 0x02000772, 0x16000f9f, 0x1800ff4f, 0x0001d630,
	};
	const u32 currents[] = { 0x20000153, 0x20000f57, 0x2000055b, 0x20000f51,
				 0x2000094a, 0x2000035f, 0x20000f53, 0x20000357,
				 0x20000353 };
	int i;
	/* Activate PMBAR.  */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), PMBASE, DEFAULT_PMBASE | 1);
	pci_write_config32(PCI_DEV(0, 0x1f, 0), PMBASE + 4, 0);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0x44 /* ACPI_CNTL */ , 0x80); /* Enable ACPI BAR */

	/* Unlock registers.  */
	outw(inw(DEFAULT_PMBASE | UPRWC) | UPRWC_WR_EN,
	     DEFAULT_PMBASE | UPRWC);

	for (i = 0; i < 14; i++)
		write32 (DEFAULT_RCBABASE + (0x3500 + 4 * i),
			 currents[portmap[i].current]);
	for (i = 0; i < 10; i++)
		write32 (DEFAULT_RCBABASE + (0x3538 + 4 * i), 0);

	for (i = 0; i < 8; i++)
		write32 (DEFAULT_RCBABASE + (0x3560 + 4 * i), rcba_dump[i]);
	for (i = 0; i < 8; i++)
		write32 (DEFAULT_RCBABASE + (0x3580 + 4 * i), 0);
	reg32 = 0;
	for (i = 0; i < 14; i++)
		if (!portmap[i].enabled)
			reg32 |= (1 << i);
	write32 (DEFAULT_RCBABASE + USBPDO, reg32);
	reg32 = 0;
	for (i = 0; i < 8; i++)
		if (portmap[i].enabled && portmap[i].oc_pin >= 0)
			reg32 |= (1 << (i + 8 * portmap[i].oc_pin));
	write32 (DEFAULT_RCBABASE + USBOCM1, reg32);
	reg32 = 0;
	for (i = 8; i < 14; i++)
		if (portmap[i].enabled && portmap[i].oc_pin >= 4)
			reg32 |= (1 << (i - 8 + 8 * (portmap[i].oc_pin - 4)));
	write32 (DEFAULT_RCBABASE + USBOCM2, reg32);
	for (i = 0; i < 22; i++)
		write32 (DEFAULT_RCBABASE + (0x35a8 + 4 * i), 0);

	pci_write_config32 (PCI_DEV (0, 0x14, 0), 0xe4, 0x00000000);

	/* Relock registers.  */
	outw(0, DEFAULT_PMBASE | UPRWC);
}
