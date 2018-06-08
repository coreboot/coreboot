/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

extern u8 bus_sr5650[14];
extern u8 bus_sp5100[2];

extern u32 apicid_sp5100;

extern u32 sbdn_sr5650;
extern u32 sbdn_sp5100;


static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int bus_isa;
	u32 apicid_sr5650;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	get_bus_conf();

	apicid_sp5100 = 0x20;
	apicid_sr5650 = apicid_sp5100 + 1;

	mptable_write_buses(mc, NULL, &bus_isa);
	/* I/O APICs:   APIC ID Version State   Address */
	{
		struct device *dev;
		u32 *dword;
		u8 byte;

		dev = dev_find_slot(0, //bus_sp5100[0], TODO: why bus_sp5100[0] use same value of bus_sr5650[0] assigned by get_pci1234(), instead of 0.
				  PCI_DEVFN(sbdn_sp5100 + 0x14, 0));
		if (dev) {
			dword = (u32 *)(pci_read_config32(dev, 0x74) & 0xfffffff0);
			smp_write_ioapic(mc, apicid_sp5100, 0x11, dword);

			/* Initialize interrupt mapping */
			/* aza */
			byte = pci_read_config8(dev, 0x63);
			byte &= 0xf8;
			byte |= 0;	/* 0: INTA, ...., 7: INTH */
			pci_write_config8(dev, 0x63, byte);

			/* SATA */
			dword = (u32 *)((pci_read_config32(dev, 0xac) &
					 ~(7 << 26)) | (6 << 26));

			/* dword |= 1 << 22; PIC and APIC co exists */
			pci_write_config32(dev, 0xac, (u32)dword);

			/*
			 * 00:12.0: PROG SATA : INT F
			 * 00:13.0: INTA USB_0
			 * 00:13.1: INTB USB_1
			 * 00:13.2: INTC USB_2
			 * 00:13.3: INTD USB_3
			 * 00:13.4: INTC USB_4
			 * 00:13.5: INTD USB2
			 * 00:14.1: INTA IDE
			 * 00:14.2: Prog HDA : INT E
			 * 00:14.5: INTB ACI
			 * 00:14.6: INTB MCI
			 */
		}
		dev = dev_find_slot(0, PCI_DEVFN(0, 0));
		if (dev) {
			pci_write_config32(dev, 0xF8, 0x1);
			dword = (u32 *)(pci_read_config32(dev, 0xFC) & 0xfffffff0);
			smp_write_ioapic(mc, apicid_sp5100+1, 0x11, dword);
		}
	}

	/* I/O Ints:    Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
#define IO_LOCAL_INT(type, intr, apicid, pin) \
	smp_write_lintsrc(mc, (type), MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, bus_isa, (intr), (apicid), (pin));

	mptable_add_isa_interrupts(mc, bus_isa, apicid_sp5100, 0);

	/* on board NIC & Slot PCIE.  */
	/* configuration B doesnt need dev 5,6,7 */
	/*
	 * PCI_INT(bus_sr5650[0x5], 0x0, 0x0, 0x11);
	 * PCI_INT(bus_sr5650[0x6], 0x0, 0x0, 0x12);
	 * PCI_INT(bus_sr5650[0x7], 0x0, 0x0, 0x13);
	 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0, (((2)<<2)|(0)), apicid_sp5100+1, 28); /* dev 2 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0, (((4)<<2)|(0)), apicid_sp5100+1, 28); /* dev 4 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0, (((12)<<2)|(0)), apicid_sp5100+1, 30); /* dev 11 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0, (((12)<<2)|(0)), apicid_sp5100+1, 30); /* dev 12 */

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_sr5650[2], (((0)<<2)|(0)), apicid_sp5100+1, 0); /* card behind dev2 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_sr5650[4], (((0)<<2)|(0)), apicid_sp5100+1, 20);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_sr5650[4], (((0)<<2)|(1)), apicid_sp5100+1, 21); /* NIC */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_sr5650[11], (((0)<<2)|(0)), apicid_sp5100+1, 8);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_sr5650[11], (((0)<<2)|(1)), apicid_sp5100+1, 9); /* card behind dev11 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_sr5650[12], (((0)<<2)|(0)), apicid_sp5100+1, 12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_sr5650[12], (((0)<<2)|(1)), apicid_sp5100+1, 13); /* card behind dev12 */

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
