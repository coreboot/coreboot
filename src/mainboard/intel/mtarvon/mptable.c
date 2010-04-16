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

/* This code is based on src/mainboard/intel/jarrell/mptable.c */

#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

static void *smp_write_config_table(void *v)
{
	static const char sig[4] = "PCMP";
	static const char oem[8] = "COREBOOT";
	static const char productid[12] = "Mt. Arvon   ";
	struct mp_config_table *mc;
	u8 bus_isa = 7;
	u8 bus_pci = 6;
	u8 bus_pcie_a = 1;

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

	/* Define bus numbers */
	smp_write_bus(mc, 0, "PCI   ");
	smp_write_bus(mc, bus_pci, "PCI   ");
	smp_write_bus(mc, bus_pcie_a, "PCI   ");
	smp_write_bus(mc, bus_isa, "ISA   ");

	/* IOAPIC handling */
	smp_write_ioapic(mc, 0x01, 0x20, 0xfec00000);

	/* ISA backward compatibility interrupts  */
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, 0x01, 0x00);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x01, 0x01, 0x01);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, 0x01, 0x02);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x03, 0x01, 0x03);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x04, 0x01, 0x04);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x06, 0x01, 0x06);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		bus_isa, 0x08, 0x01, 0x08);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x09, 0x01, 0x09);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0c, 0x01, 0x0c);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0d, 0x01, 0x0d);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0e, 0x01, 0x0e);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0f, 0x01, 0x0f);

	/* Standard local interrupt assignments */
	smp_write_lintsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, MP_APIC_ALL, 0x00);
	smp_write_lintsrc(mc, mp_NMI, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, MP_APIC_ALL, 0x01);

	/* Internal PCI devices */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x01<<2)|0, 0x01, 0x10); /* DMA controller */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x02<<2)|0, 0x01, 0x10); /* PCIe port A */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x03<<2)|0, 0x01, 0x10); /* PCIe port A1 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x1c<<2)|0, 0x01, 0x10); /* PCIe port B0 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x1c<<2)|1, 0x01, 0x11); /* PCIe port B1 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x1c<<2)|2, 0x01, 0x12); /* PCIe port B2 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x1c<<2)|3, 0x01, 0x13); /* PCIe port B3 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x1d<<2)|0, 0x01, 0x10); /* UHCI0/EHCI */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x1d<<2)|1, 0x01, 0x11); /* UHCI1 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x1e<<2)|0, 0x01, 0x10); /* Audio */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x1e<<2)|1, 0x01, 0x11); /* Modem */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x1f<<2)|1, 0x01, 0x11); /* SATA/SMBus */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			 0, (0x1f<<2)|3, 0x01, 0x13); /* ? */

	/* PCI slot */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_pci, 0x00, 0x01, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_pci, 0x01, 0x01, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_pci, 0x02, 0x01, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_pci, 0x03, 0x01, 0x13);

	/* PCIe port A slot */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_pcie_a, 0x00, 0x01, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_pcie_a, 0x01, 0x01, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_pcie_a, 0x02, 0x01, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_pcie_a, 0x03, 0x01, 0x13);

	/* There is no extension information... */

	/* Compute the checksums */
	mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);

	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
	printk(BIOS_DEBUG, "Wrote the mp table end at: %p - %p\n",
		mc, smp_next_mpe_entry(mc));
	return smp_next_mpe_entry(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr);
	return (unsigned long)smp_write_config_table(v);
}

