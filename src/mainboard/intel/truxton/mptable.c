/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int bus_isa;
	u8 bus_pea0 = 0;
	u8 bus_pea1 = 0;
	u8 bus_aioc;
	device_t dev;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	/* AIOC bridge */
	dev = dev_find_slot(0, PCI_DEVFN(0x04,0));
	if (dev) {
		bus_aioc = pci_read_config8(dev, PCI_SECONDARY_BUS);
	}
	else {
		printk(BIOS_DEBUG, "ERROR - could not find PCI 0:04.0\n");
		bus_aioc = 0;
	}
	/* PCIe A0 */
	dev = dev_find_slot(0, PCI_DEVFN(0x02,0));
	if (dev) {
		bus_pea0 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	}
	else {
		printk(BIOS_DEBUG, "ERROR - could not find PCI 0:02.0\n");
		bus_pea0 = 0;
	}
	/* PCIe A1 */
	dev = dev_find_slot(0, PCI_DEVFN(0x03,0));
	if (dev) {
		bus_pea1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	}
	else {
		printk(BIOS_DEBUG, "ERROR - could not find PCI 0:03.0\n");
		bus_pea1 = 0;
	}

	mptable_write_buses(mc, NULL, &bus_isa);

	/* IOAPIC handling */
	smp_write_ioapic(mc, 0x8, 0x20, IO_APIC_ADDR);

	mptable_add_isa_interrupts(mc, bus_isa, 0x8, 0);

	/* Standard local interrupt assignments */
	mptable_lintsrc(mc, bus_isa);

	/* IMCH/IICH PCI devices */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x01<<2)|0, 0x8, 0x10); /* DMA controller */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x02<<2)|0, 0x8, 0x10); /* PCIe port A bridge */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x03<<2)|0, 0x8, 0x10); /* PCIe port A1 bridge */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x04<<2)|0, 0x8, 0x10); /* AIOC PCI bridge */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x1d<<2)|0, 0x8, 0x10); /* UHCI/EHCI */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x1f<<2)|1, 0x8, 0x11); /* SATA/SMBus */

	if (bus_pea0) {
		/* PCIe slot 0 */
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
				 bus_pea0, (0<<2)|0, 0x8, 0x10);
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
				 bus_pea0, (0<<2)|1, 0x8, 0x11);
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
				 bus_pea0, (0<<2)|2, 0x8, 0x12);
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
				 bus_pea0, (0<<2)|3, 0x8, 0x13);
	}

	if (bus_pea1) {
		/* PCIe slots 1-4 */
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
				 bus_pea1, (0<<2)|0, 0x8, 0x10);
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
				 bus_pea1, (0<<2)|1, 0x8, 0x11);
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
				 bus_pea1, (0<<2)|2, 0x8, 0x12);
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
				 bus_pea1, (0<<2)|3, 0x8, 0x13);
	}

	if (bus_aioc) {
		/* AIOC PCI devices */
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
				 bus_aioc, (0<<2)|0, 0x8, 0x10); /* GbE0 */
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
				 bus_aioc, (1<<2)|0, 0x8, 0x11); /* GbE1 */
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
				 bus_aioc, (2<<2)|0, 0x8, 0x12); /* GbE2 */
	}

	/* There is no extension information... */

	/* Compute the checksums */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}

