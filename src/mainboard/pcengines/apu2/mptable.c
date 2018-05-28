/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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

#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <stdint.h>
#include <southbridge/amd/common/amd_pci_util.h>

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int bus_isa;

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
	u8 ioapic_id = (io_apic_read(VIO_APIC_VADDR, 0x00) >> 24);
	u8 ioapic_ver = (io_apic_read(VIO_APIC_VADDR, 0x01) & 0xFF);

	smp_write_ioapic(mc, ioapic_id, ioapic_ver, VIO_APIC_VADDR);

	/* I/O Ints:    Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
#define IO_LOCAL_INT(type, intr, apicid, pin)				\
	smp_write_lintsrc(mc, (type), MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, bus_isa, (intr), (apicid), (pin));

	/*
	 * Type 3: I/O Interrupt Table Entries:
	 * Int Type, Int Polarity, Int Level, Source Bus ID,
	 * Source Bus IRQ, Dest APIC ID, Dest PIN#
	 */

	mptable_add_isa_interrupts(mc, bus_isa, ioapic_id, 0);

	/* PCI interrupts are level triggered, and are
	 * associated with a specific bus/device/function tuple.
	 */
#define PCI_INT(bus, dev, int_sign, pin)				\
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, (bus), (((dev)<<2)|(int_sign)), ioapic_id, (pin))


	/* SMBUS / ACPI */
	PCI_INT(0x0, 0x14, 0x0, intr_data_ptr[PIRQ_SMBUS]);

	/* SD card */
	PCI_INT(0x0, 0x14, 0x1, intr_data_ptr[PIRQ_SD]);

	/* USB */
	PCI_INT(0x0, 0x12, 0x0, intr_data_ptr[PIRQ_OHCI1]);
	PCI_INT(0x0, 0x12, 0x1, intr_data_ptr[PIRQ_EHCI1]);
	PCI_INT(0x0, 0x13, 0x0, intr_data_ptr[PIRQ_OHCI2]);
	PCI_INT(0x0, 0x13, 0x1, intr_data_ptr[PIRQ_EHCI2]);
	PCI_INT(0x0, 0x16, 0x0, intr_data_ptr[PIRQ_OHCI3]);
	PCI_INT(0x0, 0x16, 0x1, intr_data_ptr[PIRQ_EHCI3]);
	PCI_INT(0x0, 0x14, 0x2, intr_data_ptr[PIRQ_OHCI4]);

	/* SATA */
	PCI_INT(0x0, 0x11, 0x0, intr_data_ptr[PIRQ_SATA]);

	/* on board NIC & Slot PCIE */
	PCI_INT(0x1, 0x0, 0x0, intr_data_ptr[PIRQ_E]);
	PCI_INT(0x2, 0x0, 0x0, intr_data_ptr[PIRQ_F]);


	/* GPP0 */
	PCI_INT(0x0, 0x2, 0x0, 0x10);	// Network 3
	/* GPP1 */
	PCI_INT(0x0, 0x2, 0x1, 0x11);	// Network 2
	/* GPP2 */
	PCI_INT(0x0, 0x2, 0x2, 0x12);	// Network 1
	/* GPP3 */
	PCI_INT(0x0, 0x2, 0x3, 0x13);	// mPCI
	/* GPP4 */
	PCI_INT(0x0, 0x2, 0x4, 0x14);	// mPCI

	IO_LOCAL_INT(mp_ExtINT, 0x0, MP_APIC_ALL, 0x0);
	IO_LOCAL_INT(mp_NMI, 0x0, MP_APIC_ALL, 0x1);

	/* There is no extension information... */

	/* Compute the checksums */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);	/* ADDR, Enable Virtual Wire */
	return (unsigned long)smp_write_config_table(v);
}
