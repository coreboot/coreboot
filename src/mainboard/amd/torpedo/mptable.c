/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
#include <string.h>
#include <stdint.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>
#include "SbPlatform.h"

#define IO_APIC_ID CONFIG_MAX_CPUS
u32 apicid_sb900;

u8 picr_data[] = {
	0x0B,0x0B,0x0B,0x0B,0x1F,0x1F,0x1F,0x1F,0xFA,0xF1,0x00,0x00,0x1F,0x1F,0x1F,0x1F,
	0x09,0x1F,0x1F,0x0B,0x1F,0x0B,0x1F,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0B,0x0B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0B,0x0B,0x0B,0x0B
};
u8 intr_data[] = {
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F,0x1F,
	0x09,0x1F,0x1F,0x10,0x1F,0x12,0x1F,0x10,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x12,0x11,0x12,0x11,0x12,0x11,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x11,0x13,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x10,0x11,0x12,0x13
};

static void smp_add_mpc_entry(struct mp_config_table *mc, unsigned length)
{
	mc->mpc_length += length;
	mc->mpc_entry_count++;
}
static void my_smp_write_bus(struct mp_config_table *mc,
	unsigned char id, const char *bustype)
{
	struct mpc_config_bus *mpc;
	mpc = smp_next_mpc_entry(mc);
	memset(mpc, '\0', sizeof(*mpc));
	mpc->mpc_type = MP_BUS;
	mpc->mpc_busid = id;
	memcpy(mpc->mpc_bustype, bustype, sizeof(mpc->mpc_bustype));
	smp_add_mpc_entry(mc, sizeof(*mpc));
}
static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int bus_isa;
	int boot_apic_id;
	unsigned apic_version;
	unsigned cpu_features;
	unsigned cpu_feature_flags;
	struct cpuid_result result;
	unsigned long cpu_flag;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);
	memcpy(mc->mpc_oem, "AMD     ", 8);

	/*Inagua used dure core CPU with one die */
	boot_apic_id = lapicid();
	apic_version = lapic_read(LAPIC_LVR) & 0xff;
	result = cpuid(1);
	cpu_features = result.eax;
	cpu_feature_flags = result.edx;
	cpu_flag = MPC_CPU_ENABLED | MPC_CPU_BOOTPROCESSOR;
	smp_write_processor(mc,
	  0, apic_version,
	  cpu_flag, cpu_features, cpu_feature_flags
	);

	cpu_flag = MPC_CPU_ENABLED;
	smp_write_processor(mc,
	  1, apic_version,
	  cpu_flag, cpu_features, cpu_feature_flags
	);

	//mptable_write_buses(mc, NULL, &bus_isa);
	my_smp_write_bus(mc, 0, "PCI   ");
	my_smp_write_bus(mc, 1, "PCI   ");
	bus_isa = 0x02;
	my_smp_write_bus(mc, bus_isa, "ISA   ");

	/* I/O APICs:   APIC ID Version State   Address */

	u8 *dword;
	u8 byte;

	ReadPMIO(SB_PMIOA_REG34, AccWidthUint32, &dword);
	dword = (u8 *)(((uintptr_t) dword) & 0xFFFFFFF0);
	/* Set IO APIC ID onto IO_APIC_ID */
	write32 (dword, 0x00);
	write32 (dword + 0x10, IO_APIC_ID << 24);
	apicid_sb900 = IO_APIC_ID;
	smp_write_ioapic(mc, apicid_sb900, 0x21, dword);

	/* PIC IRQ routine */
	for (byte = 0x0; byte < sizeof(picr_data); byte ++) {
		outb(byte, 0xC00);
		outb(picr_data[byte], 0xC01);
	}

	/* APIC IRQ routine */
	for (byte = 0x0; byte < sizeof(intr_data); byte ++) {
		outb(byte | 0x80, 0xC00);
		outb(intr_data[byte], 0xC01);
	}

	/* I/O Ints:    Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
#define IO_LOCAL_INT(type, intr, apicid, pin) \
	smp_write_lintsrc(mc, (type), MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, bus_isa, (intr), (apicid), (pin));

	//mptable_add_isa_interrupts(mc, bus_isa, apicid_sb900, 0);
	/*I/O Ints:          Type    Trigger             Polarity               Bus ID   IRQ  APIC ID       PIN# */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x0, apicid_sb900, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x1, apicid_sb900, 0x1);
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x2, apicid_sb900, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x3, apicid_sb900, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x4, apicid_sb900, 0x4);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,  0, 0x49, apicid_sb900, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x6, apicid_sb900, 0x6);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x7, apicid_sb900, 0x7);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x8, apicid_sb900, 0x8);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x9, apicid_sb900, 0x9);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,  bus_isa, 0xa, apicid_sb900, 0xa);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,  bus_isa, 0x1c, apicid_sb900, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0xc, apicid_sb900, 0xc);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0xd, apicid_sb900, 0xd);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0xe, apicid_sb900, 0xe);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0xf, apicid_sb900, 0xf);

	/* PCI interrupts are level triggered, and are
	 * associated with a specific bus/device/function tuple.
	 */
#define PCI_INT(bus, dev, int_sign, pin) \
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, (bus), (((dev)<<2)|(int_sign)), apicid_sb900, (pin))

	/* Internal VGA */
	PCI_INT(0x0, 0x01, 0x0, intr_data[0x02]);
	PCI_INT(0x0, 0x01, 0x1, intr_data[0x03]);

	/* SMBUS */
	PCI_INT(0x0, 0x14, 0x0, 0x10);

	/* HD Audio */
	PCI_INT(0x0, 0x14, 0x0, intr_data[0x13]);

	/* USB */
	PCI_INT(0x0, 0x12, 0x0, intr_data[0x30]);
	PCI_INT(0x0, 0x12, 0x1, intr_data[0x31]);
	PCI_INT(0x0, 0x13, 0x0, intr_data[0x32]);
	PCI_INT(0x0, 0x13, 0x1, intr_data[0x33]);
	PCI_INT(0x0, 0x16, 0x0, intr_data[0x34]);
	PCI_INT(0x0, 0x16, 0x1, intr_data[0x35]);
	PCI_INT(0x0, 0x14, 0x2, intr_data[0x36]);

	/* sata */
	PCI_INT(0x0, 0x11, 0x0, intr_data[0x40]);
	PCI_INT(0x0, 0x11, 0x0, intr_data[0x41]);


	/* on board NIC & Slot PCIE.  */

	/* PCI slots */
	struct device *dev = dev_find_slot(0, PCI_DEVFN(0x14, 4));
	if (dev && dev->enabled) {
		u8 bus_pci = dev->link_list->secondary;

		/* PCI_SLOT 0. */
		PCI_INT(bus_pci, 0x5, 0x0, 0x14);
		PCI_INT(bus_pci, 0x5, 0x1, 0x15);
		PCI_INT(bus_pci, 0x5, 0x2, 0x16);
		PCI_INT(bus_pci, 0x5, 0x3, 0x17);

		/* PCI_SLOT 1. */
		PCI_INT(bus_pci, 0x6, 0x0, 0x15);
		PCI_INT(bus_pci, 0x6, 0x1, 0x16);
		PCI_INT(bus_pci, 0x6, 0x2, 0x17);
		PCI_INT(bus_pci, 0x6, 0x3, 0x14);

		/* PCI_SLOT 2. */
		PCI_INT(bus_pci, 0x7, 0x0, 0x16);
		PCI_INT(bus_pci, 0x7, 0x1, 0x17);
		PCI_INT(bus_pci, 0x7, 0x2, 0x14);
		PCI_INT(bus_pci, 0x7, 0x3, 0x15);
	}

	/* PCIe Lan*/
	PCI_INT(0x0, 0x06, 0x0, 0x13);

	/* FCH PCIe PortA */
	PCI_INT(0x0, 0x15, 0x0, 0x10);
	/* FCH PCIe PortB */
	PCI_INT(0x0, 0x15, 0x1, 0x11);
	/* FCH PCIe PortC */
	PCI_INT(0x0, 0x15, 0x2, 0x12);
	/* FCH PCIe PortD */
	PCI_INT(0x0, 0x15, 0x3, 0x13);

	/*Local Ints:   Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
	IO_LOCAL_INT(mp_ExtINT, 0, MP_APIC_ALL, 0x0);
	IO_LOCAL_INT(mp_NMI, 0, MP_APIC_ALL, 0x1);
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
