/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#include <cpu/amd/amdk8_sysconf.h>

#define PCI_INT(bus, dev, fn, pin) \
	smp_write_intsrc(mc, mp_INT, \
		MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, \
		bus_mcp55[bus], (((dev) << 2) | (fn)), apicid_mcp55, (pin))

extern unsigned char bus_mcp55[8];
extern unsigned apicid_mcp55;

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	unsigned int sbdn;
	int i, j, bus_isa;
	struct device *dev;
	struct resource *res;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	get_bus_conf();
	sbdn = sysconf.sbdn;

	mptable_write_buses(mc, NULL, &bus_isa);

	dev = dev_find_slot(bus_mcp55[0], PCI_DEVFN(sbdn + 0x1, 0));
	if (dev) {
		res = find_resource(dev, PCI_BASE_ADDRESS_1);
		if (res)
			smp_write_ioapic(mc, apicid_mcp55, 0x11,
					 res2mmio(res, 0, 0));

		pci_write_config32(dev, 0x7c, 0x00000000);
		pci_write_config32(dev, 0x80, 0x11002009);
		pci_write_config32(dev, 0x84, 0x2000dd08);
	}

	mptable_add_isa_interrupts(mc, bus_isa, apicid_mcp55, 0);

	/* I/O Ints */
	PCI_INT(0, sbdn + 1, 1, 10); /* SMBus */
	PCI_INT(0, sbdn + 2, 0, 20); /* USB 1.1 */
	PCI_INT(0, sbdn + 2, 1, 22); /* USB 2.0 */
	PCI_INT(0, sbdn + 4, 0, 14); /* IDE */
	PCI_INT(0, sbdn + 5, 0, 23); /* SATA 0 */
	PCI_INT(0, sbdn + 5, 1, 23); /* SATA 1 */
	PCI_INT(0, sbdn + 5, 2, 22); /* SATA 2 */
	PCI_INT(0, sbdn + 6, 1, 21); /* HD audio */
	PCI_INT(0, sbdn + 8, 0, 24); /* NIC */

	/* PCI-E slots (two x1, one x4, one x16) */
	for (j = 7; j >= 2; j--) {
		if (!bus_mcp55[j])
			continue;
		for (i = 0; i < 4; i++)
			PCI_INT(j, 0, i, 0x10 + (2 + j + i + 4 - sbdn % 4) % 4);
	}

	/* PCI slots (three on this board) */
	for (j = 0; j < 3; j++) {
		for (i = 0; i < 4; i++)
			PCI_INT(1, 0x06 + j, i, 0x10 + (2 + i + j) % 4);
	}

	/* Local Ints:        Type       Trigger               Polarity              Bus ID   IRQ  APIC ID      PIN# */
	mptable_lintsrc(mc, bus_isa);

	/* Compute the checksums. */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
