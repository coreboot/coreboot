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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <arch/io.h>
#include <string.h>
#include <stdint.h>
#include <cpu/amd/amdfam15.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>
#include "southbridge/amd/agesa/hudson/hudson.h" /* pm_ioread() */

//-#define IO_APIC_ID    CONFIG_MAX_PHYSICAL_CPUS + 1
#define IO_APIC_ID    CONFIG_MAX_CPUS
extern u8 bus_hudson[3];

extern u32 bus_type[256];
extern u32 sbdn_hudson;
extern u32 apicid_hudson;

u8 picr_data[] = {
	0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x0A,0xF1,0x00,0x00,0x1F,0x1F,0x1F,0x1F,
	0x09,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x1F,0x1F,0x1F,0x1F
};
u8 intr_data[0x54] = {
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F,0x1F,
	0x09,0x1F,0x1F,0x10,0x1F,0x10,0x1F,0x10,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x05,0x1F,0x1F,0x1F,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
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
	u32 dword;
	u8 byte;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);
	memcpy(mc->mpc_oem, "AMD     ", 8);

	smp_write_processors(mc);

	get_bus_conf();

	//mptable_write_buses(mc, NULL, &bus_isa);
	my_smp_write_bus(mc, 0, "PCI   ");
	my_smp_write_bus(mc, 1, "PCI   ");
	bus_isa = 0x02;
	my_smp_write_bus(mc, bus_isa, "ISA   ");

	/* I/O APICs:   APIC ID Version State   Address */

	dword = 0;
	dword = pm_ioread(0x34) & 0xF0;
	dword |= (pm_ioread(0x35) & 0xFF) << 8;
	dword |= (pm_ioread(0x36) & 0xFF) << 16;
	dword |= (pm_ioread(0x37) & 0xFF) << 24;
	/* Set IO APIC ID onto IO_APIC_ID */
	write32 (dword, 0x00);
	write32 (dword + 0x10, IO_APIC_ID << 24);
	apicid_hudson = IO_APIC_ID;
	smp_write_ioapic(mc, apicid_hudson, 0x21, dword);

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
#define IO_LOCAL_INT(type, intr, apicid, pin)				\
	smp_write_lintsrc(mc, (type), MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, bus_isa, (intr), (apicid), (pin));
	mptable_add_isa_interrupts(mc, bus_isa, apicid_hudson, 0);

	/* PCI interrupts are level triggered, and are
	 * associated with a specific bus/device/function tuple.
	 */
#define PCI_INT(bus, dev, int_sign, pin)				\
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, (bus), (((dev)<<2)|(int_sign)), apicid_hudson, (pin))

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
	/* PCI_SLOT 0. */
	PCI_INT(bus_hudson[1], 0x5, 0x0, 0x14);
	PCI_INT(bus_hudson[1], 0x5, 0x1, 0x15);
	PCI_INT(bus_hudson[1], 0x5, 0x2, 0x16);
	PCI_INT(bus_hudson[1], 0x5, 0x3, 0x17);

	/* PCI_SLOT 1. */
	PCI_INT(bus_hudson[1], 0x6, 0x0, 0x15);
	PCI_INT(bus_hudson[1], 0x6, 0x1, 0x16);
	PCI_INT(bus_hudson[1], 0x6, 0x2, 0x17);
	PCI_INT(bus_hudson[1], 0x6, 0x3, 0x14);

	/* PCI_SLOT 2. */
	PCI_INT(bus_hudson[1], 0x7, 0x0, 0x16);
	PCI_INT(bus_hudson[1], 0x7, 0x1, 0x17);
	PCI_INT(bus_hudson[1], 0x7, 0x2, 0x14);
	PCI_INT(bus_hudson[1], 0x7, 0x3, 0x15);

	PCI_INT(bus_hudson[2], 0x0, 0x0, 0x12);
	PCI_INT(bus_hudson[2], 0x0, 0x1, 0x13);
	PCI_INT(bus_hudson[2], 0x0, 0x2, 0x14);

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
