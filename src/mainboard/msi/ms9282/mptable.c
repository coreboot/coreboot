/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
 *
 * Copyright (C) 2006 MSI
 * Written by Bingxun Shi <bingxunshi@gmail.com> for MSI.
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

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	get_bus_conf();
	sbdn = sysconf.sbdn;
	m = sysconf.mb;

	mptable_write_buses(mc, NULL, &bus_isa);

/*I/O APICs:   APIC ID Version State	   Address*/
	{
		struct device *dev;
		struct resource *res;
		uint32_t dword;

		dev = dev_find_slot(m->bus_mcp55[0], PCI_DEVFN(sbdn+ 0x1,0));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_1);
			if (res) {
				smp_write_ioapic(mc, m->apicid_mcp55, 0x11,
						 res2mmio(res, 0, 0));
			}

			dword = 0x43c6c643;
			pci_write_config32(dev, 0x7c, dword);

			dword = 0x81001a00;
			pci_write_config32(dev, 0x80, dword);

			dword = 0xd00002d2;
			pci_write_config32(dev, 0x84, dword);

		}


	}

	mptable_add_isa_interrupts(mc, bus_isa, m->apicid_mcp55, 0);

//SMBUS
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+1)<<2)|1, m->apicid_mcp55, 0xa);

//USB1.1
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+2)<<2)|0, m->apicid_mcp55, 0x16); // 22

//USB2.0
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+2)<<2)|1, m->apicid_mcp55, 0x17); // 23

//SATA1
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+5)<<2)|0, m->apicid_mcp55, 0x14); // 20

//SATA2
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+5)<<2)|1, m->apicid_mcp55, 0x17); // 23

//SATA3
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+5)<<2)|2, m->apicid_mcp55, 0x15); // 21

//NIC1
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+8)<<2)|0, m->apicid_mcp55, 0x16); // 22
//NIC2
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+9)<<2)|0, m->apicid_mcp55, 0x15); // 21

	for (j = 7; j >= 2; j--) {
		if (!m->bus_mcp55[j])
			continue;
		for (i = 0; i < 4; i++)
			smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[j], (0x00 << 2)|i, m->apicid_mcp55, 0x10 + (2+j+i+4-sbdn%4)%4);
	}

	for (j = 0; j < 1; j++)
		for (i = 0; i < 4; i++)
			smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[1], ((0x04+j)<<2)|i, m->apicid_mcp55, 0x10 + (2+i+j)%4);

/*Local Ints:  Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN#*/
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
