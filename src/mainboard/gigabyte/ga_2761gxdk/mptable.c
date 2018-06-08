/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
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

extern unsigned char bus_sis966[8]; //1

extern unsigned apicid_sis966;

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	unsigned sbdn;
	int i, j, bus_isa;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	get_bus_conf();
	sbdn = sysconf.sbdn;

	mptable_write_buses(mc, NULL, &bus_isa);

/*I/O APICs:	APIC ID	Version	State		Address*/
	{
		struct device *dev;
		struct resource *res;
		uint32_t dword;

		dev = dev_find_slot(bus_sis966[0], PCI_DEVFN(sbdn+ 0x1,0));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_1);
			if (res) {
				smp_write_ioapic(mc, apicid_sis966, 0x11,
						 res2mmio(res, 0, 0));
			}

			dword = 0x43c6c643;
			pci_write_config32(dev, 0x7c, dword);

			dword = 0x81001a00;
			pci_write_config32(dev, 0x80, dword);

			dword = 0xd0001202;
			pci_write_config32(dev, 0x84, dword);

		}
	}

	mptable_add_isa_interrupts(mc, bus_isa, apicid_sis966, 0);

/* PCI interrupts are level triggered, and are
 * associated with a specific bus/device/function tuple.
 */
#define PCI_INT(bus, dev, fn, pin)					\
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_sis966[bus], (((dev)<<2)|(fn)), apicid_sis966, (pin))

	PCI_INT(0, sbdn+1, 1, 0xa);
	PCI_INT(0, sbdn+2, 0, 0x16); // 22
	PCI_INT(0, sbdn+2, 1, 0x17); // 23
	PCI_INT(0, sbdn+6, 1, 0x17); // 23
	PCI_INT(0, sbdn+5, 0, 0x14); // 20
	PCI_INT(0, sbdn+5, 1, 0x17); // 23
	PCI_INT(0, sbdn+5, 2, 0x15); // 21
	PCI_INT(0, sbdn+8, 0, 0x16); // 22

	for (j = 7; j >= 2; j--) {
		if (!bus_sis966[j])
			continue;
		for (i = 0; i < 4; i++)
			PCI_INT(j, 0x00, i, 0x10 + (2+j+i+4-sbdn%4)%4);
	}

	for (j = 0; j < 2; j++)
		for (i = 0; i < 4; i++)
			PCI_INT(1, 0x06+j, i, 0x10 + (2+i+j)%4);

/*Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#*/
	mptable_lintsrc(mc, bus_isa);
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
