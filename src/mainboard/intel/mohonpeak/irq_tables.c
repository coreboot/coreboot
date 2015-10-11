/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2013, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/pirq_routing.h>

#define PIRQA 0x08
#define PIRQB 0x09
#define PIRQC 0x0a
#define PIRQD 0x0b
#define PIRQE 0x0c
#define PIRQF 0x0d
#define PIRQG 0x0e
#define PIRQH 0x0f

#define PCI_IRQS 0xDCF0

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,  /* u32 signature */
	PIRQ_VERSION,    /* u16 version   */
	32+16*CONFIG_IRQ_SLOT_COUNT,	 /* There can be total 18 devices on the bus */
	0x00,		 /* Where the interrupt router lies (bus) */
	(0x1f<<3)|0x0,   /* Where the interrupt router lies (dev) */
	0,		 /* IRQs devoted exclusively to PCI usage */
	0x8086,		 /* Vendor */
	0x0F1C,		 /* Device */
	0,		 /* miniport */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x86,		 /* u8 checksum. */
	{
		/* bus,     dev|fn,   {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap},  slot, rfu */
		{0x00,(0x01<<3)|0x0, {{PIRQA, PCI_IRQS}, {PIRQB, PCI_IRQS}, {PIRQC, PCI_IRQS}, {PIRQD, PCI_IRQS}}, 0x0, 0x0}, // PCIE Port 1: INTA-PIRQA, INTB-PIRQB, INTC-PIRQC, INTD-PIRQD
		{0x00,(0x02<<3)|0x0, {{PIRQA, PCI_IRQS}, {PIRQB, PCI_IRQS}, {PIRQC, PCI_IRQS}, {PIRQD, PCI_IRQS}}, 0x0, 0x0}, // PCIE Port 2: INTA-PIRQA, INTB-PIRQB, INTC-PIRQC, INTD-PIRQD
		{0x00,(0x03<<3)|0x0, {{PIRQE, PCI_IRQS}, {PIRQF, PCI_IRQS}, {PIRQG, PCI_IRQS}, {PIRQH, PCI_IRQS}}, 0x0, 0x0}, // PCIE Port 3: INTA-PIRQE, INTB-PIRQF, INTC-PIRQG, INTD-PIRQH
		{0x00,(0x04<<3)|0x0, {{PIRQE, PCI_IRQS}, {PIRQF, PCI_IRQS}, {PIRQG, PCI_IRQS}, {PIRQH, PCI_IRQS}}, 0x0, 0x0}, // PCIE Port 4: INTA-PIRQE, INTB-PIRQF, INTC-PIRQG, INTD-PIRQH
		{0x00,(0x0b<<3)|0x0, {{PIRQA, PCI_IRQS}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x00000}}, 0x0, 0x0}, // IQAT INTA-PIRQA
		{0x00,(0x0f<<3)|0x0, {{PIRQA, PCI_IRQS}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x00000}}, 0x0, 0x0}, // RCEC INTA-PIRQA
		{0x00,(0x13<<3)|0x0, {{PIRQA, PCI_IRQS}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x00000}}, 0x0, 0x0}, // SMBUS #1 INTA-PIRQA
		{0x00,(0x14<<3)|0x0, {{PIRQE, PCI_IRQS}, {PIRQF, PCI_IRQS}, {PIRQG, PCI_IRQS}, {PIRQH, PCI_IRQS}}, 0x0, 0x0}, // GbE, INTA-PIRQE, INTB-PIRQF, INTC-PIRQG, INTD-PIRQH
		{0x00,(0x16<<3)|0x0, {{PIRQH, PCI_IRQS}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x00000}}, 0x0, 0x0}, // EHCI INTA-PIRQH
		{0x00,(0x17<<3)|0x0, {{PIRQD, PCI_IRQS}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x00000}}, 0x0, 0x0}, // SATA2 INTA-PIRQD
		{0x00,(0x18<<3)|0x0, {{PIRQD, PCI_IRQS}, {0x00, 0x0000}, {0x00, 0x0000}, {0x00, 0x00000}}, 0x0, 0x0}, // SATA3 INTA-PIRQD
		{0x00,(0x1f<<3)|0x0, {{0x00, 0x0000}, {PIRQC, PCI_IRQS}, {0x00, 0x0000}, {0x00, 0x00000}}, 0x0, 0x0}, // LPC/SMBUS #0 INTB - PIRQC
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
