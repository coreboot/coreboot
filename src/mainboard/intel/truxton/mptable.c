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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

void *smp_write_config_table(void *v)
{
	static const char sig[4] = "PCMP";
	static const char oem[8] = "Intel   ";
	static const char productid[12] = "Truxton     ";
	struct mp_config_table *mc;
	u8 bus_num;
	u8 bus_isa;
	u8 bus_pea0 = 0;
	u8 bus_pea1 = 0;
	u8 bus_aioc;
	device_t dev;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);
	memset(mc, 0, sizeof(*mc));

	memcpy(mc->mpc_signature, sig, sizeof(sig));
	mc->mpc_length = sizeof(*mc); /* initially just the header */
	mc->mpc_spec = 0x04;
	mc->mpc_checksum = 0; /* not yet computed */
	memcpy(mc->mpc_oem, oem, sizeof(oem));
	memcpy(mc->mpc_productid, productid, sizeof(productid));
	mc->mpc_oemptr = 0;
	mc->mpc_oemsize = 0;
	mc->mpc_entry_count = 0; /* No entries yet... */
	mc->mpc_lapic = LAPIC_ADDR;
	mc->mpe_length = 0;
	mc->mpe_checksum = 0;
	mc->reserved = 0;

	smp_write_processors(mc);


	/* AIOC bridge */
	dev = dev_find_slot(0, PCI_DEVFN(0x04,0));
	if (dev) {
		bus_aioc = pci_read_config8(dev, PCI_SECONDARY_BUS);
		bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_isa++;
	}
	else {
		printk_debug("ERROR - could not find PCI 0:04.0\n");
		bus_aioc = 0;
		bus_isa = 9;
	}
	/* PCIe A0 */
	dev = dev_find_slot(0, PCI_DEVFN(0x02,0));
	if (dev) {
		bus_pea0 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	}
	else {
		printk_debug("ERROR - could not find PCI 0:02.0\n");
		bus_pea0 = 0;
	}
	/* PCIe A1 */
	dev = dev_find_slot(0, PCI_DEVFN(0x03,0));
	if (dev) {
		bus_pea1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	}
	else {
		printk_debug("ERROR - could not find PCI 0:03.0\n");
		bus_pea1 = 0;
	}

	/* define bus and isa numbers */
	for(bus_num = 0; bus_num < bus_isa; bus_num++) {
		smp_write_bus(mc, bus_num, "PCI	  ");
	}
	smp_write_bus(mc, bus_isa, "ISA	  ");

	/* IOAPIC handling */
	smp_write_ioapic(mc, 0x8, 0x20, 0xfec00000);

	/* ISA backward compatibility interrupts */
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, 0x8, 0x00);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x01, 0x8, 0x01);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, 0x8, 0x02);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x03, 0x8, 0x03);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x04, 0x8, 0x04);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x06, 0x8, 0x06);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		bus_isa, 0x08, 0x8, 0x08);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x09, 0x8, 0x09);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0c, 0x8, 0x0c);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0d, 0x8, 0x0d);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0e, 0x8, 0x0e);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0f, 0x8, 0x0f);

	/* Standard local interrupt assignments */
	smp_write_lintsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, MP_APIC_ALL, 0x00);
	smp_write_lintsrc(mc, mp_NMI, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, MP_APIC_ALL, 0x01);

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
	mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);

	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
	printk_debug("Wrote the mp table end at: %p - %p\n",
		mc, smp_next_mpe_entry(mc));
	return smp_next_mpe_entry(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr);
	return (unsigned long)smp_write_config_table(v);
}

