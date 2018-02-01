/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2001 Eric W.Biederman <ebiderman@lnxi.com>
 *
 * Copyright (C) 2006 AMD
 * Written by Yinghai Lu <yinghailu@gmail.com> for AMD.
 *
 * Copyright (C) 2006 MSI
 * Written by bxshi <bingxunshi@gmail.com> for MSI.
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
#include <arch/io.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
#include <cpu/amd/multicore.h>
#endif
#include <cpu/amd/amdk8_sysconf.h>
#include "mb_sysconf.h"

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;

	int i, bus_isa;
	struct mb_sysconf_t *m;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	get_bus_conf();
	m = sysconf.mb;

	mptable_write_buses(mc, NULL, &bus_isa);

/*I/O APICs:   APIC ID Version State           Address*/
	{
		device_t dev = 0;
		struct resource *res;
		for (i = 0; i < 3; i++) {
			dev = dev_find_device(0x1166, 0x0235, dev);
			if (dev) {
				res = find_resource(dev, PCI_BASE_ADDRESS_0);
				if (res) {
					smp_write_ioapic(mc, m->apicid_bcm5785[i], 0x11,
							res2mmio(res, 0, 0));
				}
			}
		}

	}

	mptable_add_isa_interrupts(mc, bus_isa, m->apicid_bcm5785[0], 0);

//IDE
	outb(0x02, 0xc00); outb(0x0e, 0xc01);

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		m->bus_bcm5785_0, ((1+sysconf.sbdn)<<2)|1, m->apicid_bcm5785[0], 0xe); // IDE

//SATA
	outb(0x07, 0xc00); outb(0x0f, 0xc01);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		m->bus_bcm5785_1, (0x0e << 2)|0, m->apicid_bcm5785[0], 0xf);

//USB
	outb(0x01, 0xc00); outb(0x0a, 0xc01);
	for (i = 0; i < 3; i++)
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			m->bus_bcm5785_0, ((2+sysconf.sbdn)<<2)|i, m->apicid_bcm5785[0], 0xa); //



	/* enable int */
	/* why here? must get the BAR and PCI command bit 1 set before enable it ....*/
	{
		device_t dev;
		dev = dev_find_device(0x1166, 0x0205, 0);
		if (dev) {
			uint32_t dword;
			dword = pci_read_config32(dev, 0x6c);
			dword |= (1 << 4); // enable interrupts
			pci_write_config32(dev, 0x6c, dword);
		}
		}

//First pci-x slot (on bcm5785) under bus_bcm5785_1:d.0
	// AIC 8130 Galileo Technology...
	for (i = 0; i < 4; i++)
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			m->bus_bcm5785_1_1, (6 << 2)|i, m->apicid_bcm5785[1], 2 + (1+i)%4); //


//pci slot (on bcm5785)
	for (i = 0; i < 4; i++)
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			m->bus_bcm5785_0, (5 << 2)|i, m->apicid_bcm5785[1], 8+i%4); //


//onboard ati
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		m->bus_bcm5785_0, (4 << 2)|0, m->apicid_bcm5785[1], 0x1);

//PCI-X on bcm5780
	for (i = 0; i < 4; i++)
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			m->bus_bcm5780[1], (4 << 2)|i, m->apicid_bcm5785[1], 2 + (0+i)%4); //

//onboard Broadcom
	for (i = 0; i < 2; i++)
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			m->bus_bcm5780[2], (4 << 2)|i, m->apicid_bcm5785[1], 0xa + (0+i)%4); //


// First PCI-E x8
	for (i = 0; i < 4; i++)
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			m->bus_bcm5780[5], (0 << 2)|i, m->apicid_bcm5785[1], 0xe); //


// Second PCI-E x8
	for (i = 0; i < 4; i++)
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			m->bus_bcm5780[3], (0 << 2)|i, m->apicid_bcm5785[1], 0xc); //

// Third PCI-E x1
	for (i = 0; i < 4; i++)
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
			m->bus_bcm5780[4], (0 << 2)|i, m->apicid_bcm5785[1], 0xd); //

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
