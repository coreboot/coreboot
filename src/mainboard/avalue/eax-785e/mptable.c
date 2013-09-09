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
#include <SBPLATFORM.h>
#include <cpu/amd/amdfam10_sysconf.h>

extern int bus_isa;
extern u8 bus_rs780[11];
extern u8 bus_sb800[6];
extern u32 apicid_sb800;
extern u32 bus_type[256];
extern u32 sbdn_rs780;
extern u32 sbdn_sb800;

u8 intr_data[] = {
	[0x00] = 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17, /* INTA# - INTH# */
	[0x08] = 0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F,0x1F, /* Misc-nil,0,1,2, INT from Serial irq */
	[0x10] = 0x1F,0x1F,0x1F,0x10,0x1F,0x12,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x12,0x11,0x12,0x11,0x12,0x11,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x11,0x13,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x10,0x11,0x12,0x13
};

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	u32 dword;
	u8 byte;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	get_bus_conf();

	mptable_write_buses(mc, NULL, &bus_isa);

	/* I/O APICs:   APIC ID Version State   Address */
	ReadPMIO(SB_PMIOA_REG34, AccWidthUint32, &dword);
	dword &= 0xFFFFFFF0;

	smp_write_ioapic(mc, apicid_sb800, 0x11, dword);

	for (byte = 0x0; byte < sizeof(intr_data); byte ++) {
		outb(byte | 0x80, 0xC00);
		outb(intr_data[byte], 0xC01);
	}

	/* I/O Ints:    Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
#define IO_LOCAL_INT(type, intr, apicid, pin) \
	smp_write_intsrc(mc, (type), MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, bus_isa, (intr), (apicid), (pin));

	mptable_add_isa_interrupts(mc, bus_isa, apicid_sb800, 0);

	/* PCI interrupts are level triggered, and are
	 * associated with a specific bus/device/function tuple.
	 */
#if !CONFIG_GENERATE_ACPI_TABLES
#define PCI_INT(bus, dev, fn, pin) \
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, (bus), (((dev)<<2)|(fn)), apicid_sb800, (pin))
#else
#define PCI_INT(bus, dev, fn, pin)
#endif

	PCI_INT(0x0, 0x14, 0x0, 0x10);
	/* HD Audio: */
	PCI_INT(0x0, 0x14, 0x2, 0x12);
	PCI_INT(0x0, 0x14, 0x4, 0x11);

	PCI_INT(0x0, 0x12, 0x0, intr_data[0x30]); /* USB */
	PCI_INT(0x0, 0x12, 0x1, intr_data[0x31]);
	PCI_INT(0x0, 0x13, 0x0, intr_data[0x32]);
	PCI_INT(0x0, 0x13, 0x1, intr_data[0x33]);
	PCI_INT(0x0, 0x16, 0x0, intr_data[0x34]);
	PCI_INT(0x0, 0x16, 0x1, intr_data[0x35]);

	/* sata */
	PCI_INT(0x0, 0x11, 0x0, intr_data[0x41]);

	/* on board NIC & Slot PCIE.  */
	/* PCI_INT(bus_rs780[0x1], 0x5, 0x0, 0x12); */
/* 	PCI_INT(bus_rs780[0x1], 0x5, 0x1, 0x13); */
	PCI_INT(bus_rs780[0x2], 0x0, 0x0, 0x12); /* Dev 2, external GFX */
	/* PCI_INT(bus_rs780[0x3], 0x0, 0x0, 0x13); */
	PCI_INT(bus_rs780[0x4], 0x0, 0x0, 0x10);
	/* configuration B doesnt need dev 5,6,7 */
	/*
	 * PCI_INT(bus_rs780[0x5], 0x0, 0x0, 0x11);
	 * PCI_INT(bus_rs780[0x6], 0x0, 0x0, 0x12);
	 * PCI_INT(bus_rs780[0x7], 0x0, 0x0, 0x13);
	 */
	PCI_INT(bus_rs780[0x9], 0x0, 0x0, 0x11);
	PCI_INT(bus_rs780[0xA], 0x0, 0x0, 0x12); /* NIC */

	/* PCI slots */
	/* PCI_SLOT 0. */
	PCI_INT(bus_sb800[1], 0x5, 0x0, 0x14);
	PCI_INT(bus_sb800[1], 0x5, 0x1, 0x15);
	PCI_INT(bus_sb800[1], 0x5, 0x2, 0x16);
	PCI_INT(bus_sb800[1], 0x5, 0x3, 0x17);

	/* PCI_SLOT 1. */
	PCI_INT(bus_sb800[1], 0x6, 0x0, 0x15);
	PCI_INT(bus_sb800[1], 0x6, 0x1, 0x16);
	PCI_INT(bus_sb800[1], 0x6, 0x2, 0x17);
	PCI_INT(bus_sb800[1], 0x6, 0x3, 0x14);

	/* PCI_SLOT 2. */
	PCI_INT(bus_sb800[1], 0x7, 0x0, 0x16);
	PCI_INT(bus_sb800[1], 0x7, 0x1, 0x17);
	PCI_INT(bus_sb800[1], 0x7, 0x2, 0x14);
	PCI_INT(bus_sb800[1], 0x7, 0x3, 0x15);

	/*Local Ints:   Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
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
