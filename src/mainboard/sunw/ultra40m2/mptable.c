/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
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
#include "mb_sysconf.h"

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	struct mb_sysconf_t *m;
	unsigned sbdn;
	int i, j, bus_isa;
	unsigned char apicpin[4];

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	get_bus_conf();
	sbdn = sysconf.sbdn;
	m = sysconf.mb;

	mptable_write_buses(mc, NULL, &bus_isa);

/*I/O APICs:	APIC ID	Version	State		Address*/
	{
		struct device *dev;
		struct resource *res;
		uint32_t dword;

		dev = dev_find_slot(m->bus_mcp55, PCI_DEVFN(sbdn+ 0x1,0));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_1);
			if (res)
				smp_write_ioapic(mc, m->apicid_mcp55, 0x11,
						 res2mmio(res, 0, 0));

		/* Initialize interrupt mapping*/
			dword = pci_read_config32(dev, 0x74);
			dword &= ~(1 << 15);
			dword |= 1 << 2;
			pci_write_config32(dev, 0x74, dword);

			dword = 0x43c6c643;
			pci_write_config32(dev, 0x7c, dword);

			dword = 0x81001a00;
			pci_write_config32(dev, 0x80, dword);

			dword = 0xd00012d2;
			pci_write_config32(dev, 0x84, dword);

		}

	    if (m->bus_mcp55b) {
		dev = dev_find_slot(m->bus_mcp55b, PCI_DEVFN(m->sbdnb + 0x1,0));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_1);
			if (res)
				smp_write_ioapic(mc, m->apicid_mcp55b, 0x11,
						 res2mmio(res, 0, 0));

			dword = 0x43c60000;
			pci_write_config32(dev, 0x7c, dword);

			dword = 0x81000000;
			pci_write_config32(dev, 0x80, dword);

			dword = 0xd00002d0;
			pci_write_config32(dev, 0x84, dword);

		}

	    }

	}

	mptable_add_isa_interrupts(mc, bus_isa, m->apicid_mcp55, 0);

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55, ((sbdn+1)<<2)|1, m->apicid_mcp55, 0xa); // 10

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55, ((sbdn+2)<<2)|0, m->apicid_mcp55, 0x16); // 22

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55, ((sbdn+2)<<2)|1, m->apicid_mcp55, 0x17); // 23

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55, ((sbdn+6)<<2)|1, m->apicid_mcp55, 0x17); // 23

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55, ((sbdn+5)<<2)|0, m->apicid_mcp55, 0x14); // 20
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55, ((sbdn+5)<<2)|1, m->apicid_mcp55, 0x17); // 23
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55, ((sbdn+5)<<2)|2, m->apicid_mcp55, 0x15); // 21

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55, ((sbdn+8)<<2)|0, m->apicid_mcp55, 0x16); // 22
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55, ((sbdn+9)<<2)|0, m->apicid_mcp55, 0x15); // 21

//Slot PCIE
	for (j = 2; j < 8; j++) {
		struct device *dev;
		dev = dev_find_slot(m->bus_mcp55, PCI_DEVFN(sbdn + 0x0a + j - 2 , 0));
		if (!dev || !dev->enabled)
			continue;
		for (i = 0; i < 4; i++)
			apicpin[i] = 0x10 + (2+j+i+4-sbdn%4)%4;
		smp_write_intsrc_pci_bridge(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, dev, m->apicid_mcp55, apicpin);
	}

//Slot PCI 32
	{
		struct device *dev;
		dev = dev_find_slot(m->bus_mcp55, PCI_DEVFN(sbdn + 6 , 0));
		if (dev && dev->enabled) {
			for (i = 0; i < 4; i++)
				apicpin[i] = 0x10 + i%4;
			smp_write_intsrc_pci_bridge(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, dev, m->apicid_mcp55, apicpin);
		}
	}

	if (m->bus_mcp55b) {
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55b, ((m->sbdnb+5)<<2)|0, m->apicid_mcp55b, 0x14); // 20
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55b, ((m->sbdnb+5)<<2)|1, m->apicid_mcp55b, 0x17); // 23
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55b, ((m->sbdnb+5)<<2)|2, m->apicid_mcp55b, 0x15); // 21

		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55b, ((m->sbdnb+8)<<2)|0, m->apicid_mcp55b, 0x16); // 22
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55b, ((m->sbdnb+9)<<2)|0, m->apicid_mcp55b, 0x15); // 21


	//Slot  PCIE
		for (j = 2; j < 8; j++) {
			struct device *dev;
			dev = dev_find_slot(m->bus_mcp55b, PCI_DEVFN(m->sbdnb + 0x0a + j - 2 , 0));
			if (!dev || !dev->enabled)
				continue;
			for (i = 0; i < 4; i++) {
				apicpin[i] = 0x10 + (2+j+i+4-m->sbdnb%4)%4;
			}
			smp_write_intsrc_pci_bridge(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, dev, m->apicid_mcp55b, apicpin);
		}

	}

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
