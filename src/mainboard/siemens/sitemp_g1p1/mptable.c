/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

#include <cpu/amd/amdk8_sysconf.h>

extern u8 bus_rs690[8];
extern u8 bus_sb600[2];

extern u32 apicid_sb600;

extern u32 sbdn_rs690;
extern u32 sbdn_sb600;

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int isa_bus;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);
	mptable_init(mc, LOCAL_APIC_ADDR);
	smp_write_processors(mc);

	get_bus_conf();
	printk(BIOS_DEBUG, "%s: apic_id=0x%x\n", __func__, apicid_sb600);

	mptable_write_buses(mc, NULL, &isa_bus);
	/* I/O APICs:   APIC ID Version State   Address */
	{
		struct device *dev;

		dev = dev_find_slot(bus_sb600[0], PCI_DEVFN(sbdn_sb600 + 0x14, 0));
		if (dev) {
			struct resource *res;
			res = find_resource(dev, 0x74);
			smp_write_ioapic(mc, apicid_sb600, 0x20,
					 res2mmio(res, 0, 0));
		}
	}
	mptable_add_isa_interrupts(mc, isa_bus, apicid_sb600, 0);

#define PCI_INT(bus, dev, fn, pin) \
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, (bus), (((dev)<<2)|(fn)), apicid_sb600, (pin))

	/* usb */
	PCI_INT(0x0, 0x13, 0x0, 0x10);
	PCI_INT(0x0, 0x13, 0x1, 0x11);
	PCI_INT(0x0, 0x13, 0x2, 0x12);
	PCI_INT(0x0, 0x13, 0x3, 0x13);

	/* sata */
	PCI_INT(0x0, 0x12, 0x1, 0x16);

	/* SMBus/ACPI */
	PCI_INT(0x0, 0x14, 0x0, 0x10);
	/* IDE */
	PCI_INT(0x0, 0x14, 0x1, 0x11);
	/* HDA */
	PCI_INT(0x0, 0x14, 0x2, 0x12);
	/* LPC */
	PCI_INT(0x0, 0x14, 0x3, 0x13);

	/* GFX ? */
	PCI_INT(bus_rs690[1], 0x5, 0x0, 0x12);
	PCI_INT(bus_rs690[1], 0x5, 0x1, 0x13);

	/* PCIe slots */
	PCI_INT(0x2, 0x00, 0x00, 0x10);
	PCI_INT(0x2, 0x00, 0x01, 0x11);
	PCI_INT(0x2, 0x00, 0x02, 0x12);
	PCI_INT(0x2, 0x00, 0x03, 0x13);

	/* PCIe slots */
	PCI_INT(0x3, 0x00, 0x00, 0x11);
	PCI_INT(0x3, 0x00, 0x01, 0x12);
	PCI_INT(0x3, 0x00, 0x02, 0x13);
	PCI_INT(0x3, 0x00, 0x03, 0x10);

	/* PCIe slots */
	PCI_INT(0x4, 0x00, 0x00, 0x12);
	PCI_INT(0x4, 0x00, 0x01, 0x13);
	PCI_INT(0x4, 0x00, 0x02, 0x10);
	PCI_INT(0x4, 0x00, 0x03, 0x11);

	/* PCIe slots */
	PCI_INT(0x5, 0x00, 0x00, 0x13);
	PCI_INT(0x5, 0x00, 0x01, 0x10);
	PCI_INT(0x5, 0x00, 0x02, 0x11);
	PCI_INT(0x5, 0x00, 0x03, 0x12);

	/* onboard NIC ? */
	PCI_INT(bus_sb600[1], 0x7, 0x0, 0x13);
	PCI_INT(bus_sb600[1], 0x7, 0x1, 0x10);
	PCI_INT(bus_sb600[1], 0x7, 0x2, 0x11);
	PCI_INT(bus_sb600[1], 0x7, 0x3, 0x12);

	/* I/O Ints:    Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
	mptable_lintsrc(mc, isa_bus);

	/* Compute the checksums */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
