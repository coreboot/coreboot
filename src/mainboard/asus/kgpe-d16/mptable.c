/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Raptor Engineering
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
	struct device *dev;
	uint8_t sp5100_bus_number;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	get_bus_conf();

	if (IS_ENABLED(CONFIG_ENABLE_APIC_EXT_ID) && (CONFIG_APIC_ID_OFFSET > 0))
		apicid_sp5100 = 0x0;
	else
		apicid_sp5100 = 0x20;
	apicid_sr5650 = apicid_sp5100 + 1;

	mptable_write_buses(mc, NULL, &bus_isa);
	/* I/O APICs:   APIC ID Version State   Address */
	{
		uint32_t *dword_ptr;
		uint32_t dword;
		uint16_t word;
		uint8_t byte;

		sp5100_bus_number = 0; //bus_sp5100[0]; TODO: why bus_sp5100[0] use same value of bus_sr5650[0] assigned by get_pci1234(), instead of 0.

		dev = dev_find_slot(sp5100_bus_number, PCI_DEVFN(sbdn_sp5100 + 0x14, 0));
		if (dev) {
			dword_ptr = (u32 *)(pci_read_config32(dev, 0x74) & 0xfffffff0);
			smp_write_ioapic(mc, apicid_sp5100, 0x11, dword_ptr);

			/* Initialize interrupt mapping */
			/* USB 1 & 2 */
			word = pci_read_config16(dev, 0xbe);
			word &= ~0x3f3f;
			word |= 0x0;		/* 0: INTA, ...., 7: INTH */
			word |= (0x1 << 3);	/* 0: INTA, ...., 7: INTH */
			word |= (0x2 << 8);	/* 0: INTA, ...., 7: INTH */
			word |= (0x3 << 11);	/* 0: INTA, ...., 7: INTH */
			pci_write_config16(dev, 0xbe, word);

			/* USB 3 */
			byte = pci_read_config8(dev, 0x63);
			byte &= 0xf8;
			byte |= (0x2 << 4);	/* 0: INTA, ...., 7: INTH */
			pci_write_config8(dev, 0x63, byte);

			dword = pci_read_config32(dev, 0xac);

			/* SATA */
			dword &= ~(7 << 26);
			dword |= (0x6 << 26);	/* 0: INTA, ...., 7: INTH */

			/* Hide IDE */
			dword &= ~(0x00080000);

			/* dword_ptr |= 1 << 22; PIC and APIC co exists */
			pci_write_config32(dev, 0xac, dword);

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
			dword_ptr = (u32 *)(pci_read_config32(dev, 0xFC) & 0xfffffff0);
			smp_write_ioapic(mc, apicid_sr5650, 0x11, dword_ptr);
		}
	}

	/* I/O Ints:    Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
#define IO_LOCAL_INT(type, intr, apicid, pin) \
	smp_write_lintsrc(mc, (type), MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, bus_isa, (intr), (apicid), (pin));

	mptable_add_isa_interrupts(mc, bus_isa, apicid_sp5100, 0);

	/* SR5650 devices */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0, (((0)<<2)|(2)), apicid_sr5650, 31);	/* Device 0 Function 2 (LNKA, APIC pin 31) */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0, (((2)<<2)|(0)), apicid_sr5650, 28);	/* Device 2 (LNKE, APIC pin 28) */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0, (((4)<<2)|(0)), apicid_sr5650, 28);	/* Device 4 (LNKF, APIC pin 28) */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0, (((9)<<2)|(0)), apicid_sr5650, 29);	/* Device 9 (LNKG, APIC pin 29) */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0, (((10)<<2)|(0)), apicid_sr5650, 30);	/* Device 10 (LNKG, APIC pin 30) */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0, (((11)<<2)|(0)), apicid_sr5650, 30);	/* Device 11 (LNKG, APIC pin 30) */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0, (((12)<<2)|(0)), apicid_sr5650, 30);	/* Device 12 (LNKG, APIC pin 30) */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0, (((13)<<2)|(0)), apicid_sr5650, 30);	/* Device 13 (LNKG, APIC pin 30)) */

	dev = dev_find_slot(0, PCI_DEVFN(0x2, 0));
	if (dev && dev->enabled) {
		uint8_t bus_pci = dev->link_list->secondary;
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pci, (((0)<<0x2)|(0)), apicid_sr5650, 0);	/* card behind dev2 */
	}
	dev = dev_find_slot(0, PCI_DEVFN(0x4, 0));
	if (dev && dev->enabled) {
		uint8_t bus_pci = dev->link_list->secondary;
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pci, (((0)<<0x4)|(0)), apicid_sr5650, 0);	/* PIKE */
	}
	dev = dev_find_slot(0, PCI_DEVFN(0x9, 0));
	if (dev && dev->enabled) {
		uint8_t bus_pci = dev->link_list->secondary;
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pci, (((0)<<0x9)|(0)), apicid_sr5650, 23);	/* NIC A */
	}
	dev = dev_find_slot(0, PCI_DEVFN(0xa, 0));
	if (dev && dev->enabled) {
		uint8_t bus_pci = dev->link_list->secondary;
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pci, (((0)<<0xa)|(0)), apicid_sr5650, 24);	/* NIC B */
	}
	dev = dev_find_slot(0, PCI_DEVFN(0xb, 0));
	if (dev && dev->enabled) {
		uint8_t bus_pci = dev->link_list->secondary;
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pci, (((0)<<0xb)|(0)), apicid_sr5650, 0);	/* card behind dev11 */
	}
	dev = dev_find_slot(0, PCI_DEVFN(0xc, 0));
	if (dev && dev->enabled) {
		uint8_t bus_pci = dev->link_list->secondary;
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pci, (((0)<<0xc)|(0)), apicid_sr5650, 0);	/* card behind dev12 */
	}
	dev = dev_find_slot(0, PCI_DEVFN(0xd, 0));
	if (dev && dev->enabled) {
		uint8_t bus_pci = dev->link_list->secondary;
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pci, (((0)<<0xd)|(0)), apicid_sr5650, 0);	/* card behind dev13 */
	}

	/* PCI interrupts are level triggered, and are
	 * associated with a specific bus/device/function tuple.
	 */
#define PCI_INT(bus, dev, interrupt_signal, pin) \
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, (bus), (((dev)<<2)|(interrupt_signal)), apicid_sp5100, (pin))

	/* USB1 */
	PCI_INT(sp5100_bus_number, 0x12, 0x0, 0x10); /* OHCI0 Port 0~2 */
	PCI_INT(sp5100_bus_number, 0x12, 0x1, 0x11); /* OHCI1 Port 3~5 */

	/* USB2 */
	PCI_INT(sp5100_bus_number, 0x13, 0x0, 0x12); /* OHCI0 Port 6~8 */
	PCI_INT(sp5100_bus_number, 0x13, 0x1, 0x13); /* EHCI Port 6~11 */

	/* USB3 */
	PCI_INT(sp5100_bus_number, 0x14, 0x3, 0x12); /* OHCI0 Port 12~13 */

	/* SATA */
	PCI_INT(sp5100_bus_number, 0x11, 0x0, 0x16); /* 6, INTG */

	/* PCI slots */
	dev = dev_find_slot(0, PCI_DEVFN(0x14, 4));
	if (dev && dev->enabled) {
		u8 bus_pci = dev->link_list->secondary;

		/* PCI_SLOT 0. */
		PCI_INT(bus_pci, 0x1, 0x0, 0x15);
		PCI_INT(bus_pci, 0x1, 0x1, 0x16);
		PCI_INT(bus_pci, 0x1, 0x2, 0x17);
		PCI_INT(bus_pci, 0x1, 0x3, 0x14);

		/* PCI_SLOT 1. */
		PCI_INT(bus_pci, 0x2, 0x0, 0x14);
		PCI_INT(bus_pci, 0x2, 0x1, 0x15);
		PCI_INT(bus_pci, 0x2, 0x2, 0x16);
		PCI_INT(bus_pci, 0x2, 0x3, 0x17);

		/* PCI_SLOT 2. */
		PCI_INT(bus_pci, 0x3, 0x0, 0x16);
		PCI_INT(bus_pci, 0x3, 0x1, 0x17);
		PCI_INT(bus_pci, 0x3, 0x2, 0x14);
		PCI_INT(bus_pci, 0x3, 0x3, 0x15);
	}

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
