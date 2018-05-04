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

#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <string.h>
#include <stdint.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>
#include <southbridge/amd/common/amd_pci_util.h>
#include <drivers/generic/ioapic/chip.h>

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int bus_isa;

	/*
	 * By the time this function gets called, the IOAPIC registers
	 * have been written so they can be read to get the correct
	 * APIC ID and Version
	 */
	u8 ioapic_id = (io_apic_read(VIO_APIC_VADDR, 0x00) >> 24);
	u8 ioapic_ver = (io_apic_read(VIO_APIC_VADDR, 0x01) & 0xFF);

	/* Intialize the MP_Table */
	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	/*
	 * Type 0: Processor Entries:
	 * LAPIC ID, LAPIC Version, CPU Flags:EN/BP,
	 * CPU Signature (Stepping, Model, Family),
	 * Feature Flags
	 */
	smp_write_processors(mc);

	/*
	 * Type 1: Bus Entries:
	 * Bus ID, Bus Type
	 */
	mptable_write_buses(mc, NULL, &bus_isa);

	/*
	 * Type 2: I/O APICs:
	 * APIC ID, Version, APIC Flags:EN, Address
	 */
	smp_write_ioapic(mc, ioapic_id, ioapic_ver, VIO_APIC_VADDR);

	/*
	 * Type 3: I/O Interrupt Table Entries:
	 * Int Type, Int Polarity, Int Level, Source Bus ID,
	 * Source Bus IRQ, Dest APIC ID, Dest PIN#
	 */
	mptable_add_isa_interrupts(mc, bus_isa, ioapic_id, 0);

	/* PCI interrupts are level triggered, and are
	 * associated with a specific bus/device/function tuple.
	 */
#define PCI_INT(bus, dev, fn, pin) \
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, (bus), (((dev)<<2)|(fn)), ioapic_id, (pin))

	/* APU Internal Graphic Device */
	PCI_INT(0x0, 0x01, 0x0, intr_data_ptr[PIRQ_C]);
	PCI_INT(0x0, 0x01, 0x1, intr_data_ptr[PIRQ_D]);

	/* SMBUS / ACPI */
	PCI_INT(0x0, 0x14, 0x0, intr_data_ptr[PIRQ_SMBUS]);

	/* Southbridge HD Audio */
	PCI_INT(0x0, 0x14, 0x2, intr_data_ptr[PIRQ_HDA]);

	/* LPC */
	PCI_INT(0x0, 0x14, 0x3, intr_data_ptr[PIRQ_C]);

	/* USB */
	PCI_INT(0x0, 0x12, 0x0, intr_data_ptr[PIRQ_OHCI1]);
	PCI_INT(0x0, 0x12, 0x2, intr_data_ptr[PIRQ_EHCI1]);
	PCI_INT(0x0, 0x13, 0x0, intr_data_ptr[PIRQ_OHCI2]);
	PCI_INT(0x0, 0x13, 0x2, intr_data_ptr[PIRQ_EHCI2]);
	PCI_INT(0x0, 0x16, 0x0, intr_data_ptr[PIRQ_OHCI3]);
	PCI_INT(0x0, 0x16, 0x2, intr_data_ptr[PIRQ_EHCI3]);
	PCI_INT(0x0, 0x14, 0x2, intr_data_ptr[PIRQ_OHCI4]);

	/* SATA */
	PCI_INT(0x0, 0x11, 0x0, intr_data_ptr[PIRQ_SATA]);

	/* on board NIC & Slot PCIE */
	PCI_INT(0x1, 0x0, 0x0, intr_data_ptr[PIRQ_E]);
	PCI_INT(0x2, 0x0, 0x0, intr_data_ptr[PIRQ_F]);

	/* PCI slots */
	struct device *dev = dev_find_slot(0, PCI_DEVFN(0x14, 4));
	if (dev && dev->enabled) {
		u8 bus_pci = dev->link_list->secondary;
		/* PCI_SLOT 0 */
		PCI_INT(bus_pci, 0x5, 0x0, intr_data_ptr[PIRQ_E]);
		PCI_INT(bus_pci, 0x5, 0x1, intr_data_ptr[PIRQ_F]);
		PCI_INT(bus_pci, 0x5, 0x2, intr_data_ptr[PIRQ_G]);
		PCI_INT(bus_pci, 0x5, 0x3, intr_data_ptr[PIRQ_H]);

		/* PCI_SLOT 1 */
		PCI_INT(bus_pci, 0x6, 0x0, intr_data_ptr[PIRQ_F]);
		PCI_INT(bus_pci, 0x6, 0x1, intr_data_ptr[PIRQ_G]);
		PCI_INT(bus_pci, 0x6, 0x2, intr_data_ptr[PIRQ_H]);
		PCI_INT(bus_pci, 0x6, 0x3, intr_data_ptr[PIRQ_E]);

		/* PCI_SLOT 2 */
		PCI_INT(bus_pci, 0x7, 0x0, intr_data_ptr[PIRQ_G]);
		PCI_INT(bus_pci, 0x7, 0x1, intr_data_ptr[PIRQ_H]);
		PCI_INT(bus_pci, 0x7, 0x2, intr_data_ptr[PIRQ_E]);
		PCI_INT(bus_pci, 0x7, 0x3, intr_data_ptr[PIRQ_F]);

		PCI_INT(bus_pci, 0x0, 0x0, intr_data_ptr[PIRQ_C]);
		PCI_INT(bus_pci, 0x0, 0x1, intr_data_ptr[PIRQ_D]);
		PCI_INT(bus_pci, 0x0, 0x2, intr_data_ptr[PIRQ_E]);
	}

	/* PCIe Lan*/
	PCI_INT(0x0, 0x06, 0x0, intr_data_ptr[PIRQ_D]);

	/* FCH PCIe PortA */
	PCI_INT(0x0, 0x15, 0x0, intr_data_ptr[PIRQ_A]);
	/* FCH PCIe PortB */
	PCI_INT(0x0, 0x15, 0x1, intr_data_ptr[PIRQ_B]);
	/* FCH PCIe PortC */
	PCI_INT(0x0, 0x15, 0x2, intr_data_ptr[PIRQ_C]);
	/* FCH PCIe PortD */
	PCI_INT(0x0, 0x15, 0x3, intr_data_ptr[PIRQ_D]);

	/*Local Ints:	 Type	Polarity	Trigger	 Bus ID	 IRQ	APIC ID PIN# */
#define IO_LOCAL_INT(type, intr, apicid, pin) \
		smp_write_lintsrc(mc, (type), MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, bus_isa, (intr), (apicid), (pin));

	IO_LOCAL_INT(mp_ExtINT, 0x0, MP_APIC_ALL, 0x0);
	IO_LOCAL_INT(mp_NMI, 0x0, MP_APIC_ALL, 0x1);
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
