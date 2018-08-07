/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 - 2014 Advanced Micro Devices, Inc.
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
#include <arch/ioapic.h>
#include <southbridge/amd/pi/hudson/amd_pci_int_defs.h>
#include <northbridge/amd/pi/00630F01/pci_devs.h>

#define NB_APIC_ADDR ((u8 *)0xFEC20000)

#define PCI_INT(bus, dev, fn, apic, pin) \
		if (((pin) != 0x00) && ((pin) != 0x1F)) \
		{ \
			smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, (bus), (((dev)<<2)|(fn)), apic, (pin)); \
		}

#define IO_LOCAL_INT(type, intr, apicid, pin) \
		smp_write_lintsrc(mc, (type), MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, bus_isa, (intr), (apicid), (pin));

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int bus_isa;

	/* Initialize the MP_Table */
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

	u8 apicid_nb = (io_apic_read(NB_APIC_ADDR, 0x00) >> 24);	/* Get the GNB IOAPIC ID */
	u8 apicver_nb = (io_apic_read(NB_APIC_ADDR, 0x01) & 0xFF);	/* Get the GNB IOAPIC version */

	smp_write_ioapic(mc, apicid_nb, apicver_nb, NB_APIC_ADDR);

	u8 apicid_sb = (io_apic_read(VIO_APIC_VADDR, 0x00) >> 24);		/* Get the southbridge IOAPIC ID */
	u8 apicver_sb = (io_apic_read(VIO_APIC_VADDR, 0x01) & 0xFF);	/* Get the southbridge IOAPIC version */

	smp_write_ioapic(mc, apicid_sb, apicver_sb, VIO_APIC_VADDR);

	/*
	 * Type 3: I/O Interrupt Table Entries:
	 * Int Type, Int Polarity, Int Level, Source Bus ID,
	 * Source Bus IRQ, Dest APIC ID, Dest PIN#
	 */
	mptable_add_isa_interrupts(mc, bus_isa, apicid_nb, 0);
	mptable_add_isa_interrupts(mc, bus_isa, apicid_sb, 0);

	/* PCI interrupts are level triggered, and are
	 * associated with a specific bus/device/function tuple.
	 */

	/* APU Internal Graphic Device */
	PCI_INT(0x0, 0x01, 0x0, apicid_nb, intr_data_ptr[PIRQ_GFX]);
	PCI_INT(0x0, 0x01, 0x1, apicid_nb, intr_data_ptr[PIRQ_ACTL]);

	/* SMBUS / ACPI */
	PCI_INT(0x0, 0x14, 0x0, apicid_sb, intr_data_ptr[PIRQ_SMBUS]);

	/* Southbridge HD Audio */
	PCI_INT(0x0, 0x14, 0x2, apicid_sb, intr_data_ptr[PIRQ_HDA]);

	/* USB */
	PCI_INT(0x0, 0x12, 0x0, apicid_sb, intr_data_ptr[PIRQ_OHCI1]);
	PCI_INT(0x0, 0x12, 0x2, apicid_sb, intr_data_ptr[PIRQ_EHCI1]);
	PCI_INT(0x0, 0x13, 0x0, apicid_sb, intr_data_ptr[PIRQ_OHCI2]);
	PCI_INT(0x0, 0x13, 0x2, apicid_sb, intr_data_ptr[PIRQ_EHCI2]);
	PCI_INT(0x0, 0x16, 0x0, apicid_sb, intr_data_ptr[PIRQ_OHCI3]);
	PCI_INT(0x0, 0x16, 0x2, apicid_sb, intr_data_ptr[PIRQ_EHCI3]);
	PCI_INT(0x0, 0x14, 0x5, apicid_sb, intr_data_ptr[PIRQ_OHCI4]);

	/* SATA */
	PCI_INT(0x0, 0x11, 0x0, apicid_sb, intr_data_ptr[PIRQ_SATA]);

	/* IDE */
	PCI_INT(0x0, 0x14, 0x1, apicid_sb, intr_data_ptr[PIRQ_IDE]);

	/* PCI slots */
	/* NB Gfx PCIe Bridges */
	PCI_INT(0, 0x2, 0x1, apicid_nb, intr_data_ptr[PIRQ_A]);
	PCI_INT(0, 0x2, 0x2, apicid_nb, intr_data_ptr[PIRQ_A]);

	/* NB GPP PCIe Bridges */
	PCI_INT(0, 0x3, 0x1, apicid_nb, intr_data_ptr[PIRQ_A]);
	PCI_INT(0, 0x3, 0x2, apicid_nb, intr_data_ptr[PIRQ_A]);
	PCI_INT(0, 0x3, 0x3, apicid_nb, intr_data_ptr[PIRQ_A]);
	PCI_INT(0, 0x3, 0x4, apicid_nb, intr_data_ptr[PIRQ_A]);
	PCI_INT(0, 0x3, 0x5, apicid_nb, intr_data_ptr[PIRQ_A]);

	/* PCI slots */
	PCI_INT(0, 0x14, 0x4, apicid_sb, intr_data_ptr[PIRQ_A]);

	/* FCH GPP PCIe Bridges */
	PCI_INT(0x0, 0x15, 0x0, apicid_sb, intr_data_ptr[PIRQ_A]);
	/* FCH PCIe PortB */
	PCI_INT(0x0, 0x15, 0x1, apicid_sb, intr_data_ptr[PIRQ_A]);
	/* FCH PCIe PortC */
	PCI_INT(0x0, 0x15, 0x2, apicid_sb, intr_data_ptr[PIRQ_A]);
	/* FCH PCIe PortD */
	PCI_INT(0x0, 0x15, 0x3, apicid_sb, intr_data_ptr[PIRQ_A]);

	/*Local Ints:	 Type	Polarity	Trigger	 Bus ID	 IRQ	APIC ID PIN# */
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
