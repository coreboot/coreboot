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
#include <cpu/amd/amdfam10_sysconf.h>

extern int bus_isa;
extern u8 bus_rs780[11];
extern u8 bus_sb800[6];
extern u32 apicid_sb800;
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
	u32 dword = 0;
	u8 byte;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	get_bus_conf();

	mptable_write_buses(mc, NULL, &bus_isa);

	/* I/O APICs:   APIC ID Version State   Address */
	// FIXME: ReadPMIO(SB_PMIOA_REG34, AccWidthUint32, &dword);
	dword &= 0xFFFFFFF0;

	smp_write_ioapic(mc, apicid_sb800, 0x11,(void *) dword);

	for (byte = 0x0; byte < sizeof(intr_data); byte ++) {
		outb(byte | 0x80, 0xC00);
		outb(intr_data[byte], 0xC01);
	}

	/* I/O Ints:    Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
#define IO_LOCAL_INT(type, intr, apicid, pin) \
	smp_write_intsrc(mc, (type), MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, bus_isa, (intr), (apicid), (pin));

	mptable_add_isa_interrupts(mc, bus_isa, apicid_sb800, 0);


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
