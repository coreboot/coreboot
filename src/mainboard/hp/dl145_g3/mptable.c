/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2001 Eric W.Biederman<ebiderman@lnxi.com>
 *
 * Copyright (C) 2006 AMD
 * Written by Yinghai Lu <yinghailu@gmail.com> for AMD.
 *
 * Copyright (C) 2007 University of Mannheim
 * Written by Philipp Degler <pdegler@rumms.uni-mannheim.e> for Uni of Mannheim
 *
 * Copyright (C) 2009 University of Heidelberg
 * Written by Mondrian Nuessle <nuessle@uni-hd.de> for Uni of Heidelberg
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/io.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#if CONFIG_LOGICAL_CPUS==1
#include <cpu/amd/multicore.h>
#endif
#include <cpu/amd/amdk8_sysconf.h>
#include "mb_sysconf.h"

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	struct mb_sysconf_t *m;
	int bus_isa;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LAPIC_ADDR);

	smp_write_processors(mc);

	get_bus_conf();
	m = sysconf.mb;

	mptable_write_buses(mc, NULL, &bus_isa);

	/*I/O APICs:   APIC ID Version State           Address*/
	{
		device_t dev = 0;
		int i;
		struct resource *res;
		for(i=0; i<3; i++) {
			dev = dev_find_device(0x1166, 0x0235, dev);
			if (dev) {
				res = find_resource(dev, PCI_BASE_ADDRESS_0);
				if (res) {
					printk(BIOS_DEBUG, "APIC %d base address: %llx\n",m->apicid_bcm5785[i],  res->base);
					smp_write_ioapic(mc, m->apicid_bcm5785[i], 0x11, res->base);
				}
			}
		}

	}

	/* IRQ routing as factory BIOS */
	outb(0x01, 0xc00); outb(0x0A, 0xc01);
	outb(0x17, 0xc00); outb(0x05, 0xc01);
/* 	outb(0x2E, 0xc00); outb(0x0B, 0xc01); */
/* 	outb(0x07, 0xc00); outb(0x07, 0xc01); */
	outb(0x07, 0xc00); outb(0x0b, 0xc01);

	outb(0x24, 0xc00); outb(0x05, 0xc01);
	//outb(0x00, 0xc00); outb(0x09, 0xc01);
	outb(0x02, 0xc00); outb(0x0E, 0xc01);

	// 8259 registers...
	outb(0xa0, 0x4d0);
	outb(0x0e, 0x4d1);

	{
		device_t dev;
		dev = dev_find_device(0x1166, 0x0205, 0);
		if(dev) {
			uint32_t dword;
			dword = pci_read_config32(dev, 0x64);
			dword |= (1<<30); // GEVENT14-21 used as PCI IRQ0-7
			pci_write_config32(dev, 0x64, dword);
		}
		// set GEVENT pins to NO OP
		outb(0x33, 0xcd6); outb(0x00, 0xcd7);
		outb(0x34, 0xcd6); outb(0x00, 0xcd7);
		outb(0x35, 0xcd6); outb(0x00, 0xcd7);
	}

	// hide XIOAPIC PCI configuration space
	{
		device_t dev;
		dev = dev_find_device(0x1166, 0x205, 0);
		if (dev) {
			uint32_t dword;
			dword = pci_read_config32(dev, 0x64);
			dword |= (1<<26);
			pci_write_config32(dev, 0x64, dword);
		}
	}

	mptable_add_isa_interrupts(mc, bus_isa, m->apicid_bcm5785[0], 0);

	//SATA
/* 	printk(BIOS_DEBUG, "MPTABLE_SATA: bus_id:%d irq:%d apic_id:%d pin:%d\n",m->bus_bcm5785_1, (0x0e<<2)|0, m->apicid_bcm5785[0], 0x7); */
/*	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_bcm5785_1, (0x0e<<2)|0, m->apicid_bcm5785[0], 0x7); */
	printk(BIOS_DEBUG, "MPTABLE_SATA: bus_id:%d irq:%d apic_id:%d pin:%d\n",m->bus_bcm5785_1, (0x0e<<2)|0, m->apicid_bcm5785[0], 0xb);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_bcm5785_1, (0x0e<<2)|0, m->apicid_bcm5785[0], 0xb);
	//USB
	printk(BIOS_DEBUG, "sysconf.sbdn: %d on bus: %x \n",sysconf.sbdn, m->bus_bcm5785_0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_bcm5785_0, (0x03<<2)|0, m->apicid_bcm5785[0], 0xa);

	//VGA
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_bcm5785_0, (0x4<<2)|0, m->apicid_bcm5785[1], 0x7);

	//PCIE
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_bcm5785_0, (0x6<<2)|0, m->apicid_bcm5785[2], 0xe);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_bcm5785_0, (0x7<<2)|0, m->apicid_bcm5785[2], 0xe);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_bcm5785_0, (0x8<<2)|0, m->apicid_bcm5785[2], 0xe);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_bcm5785_0, (0x9<<2)|0, m->apicid_bcm5785[2], 0xe);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_bcm5785_0, (0xa<<2)|0, m->apicid_bcm5785[2], 0xe);

	//IDE
//     	outb(0x02, 0xc00); outb(0x0e, 0xc01);
//	printk(BIOS_DEBUG, "MPTABLE_IDE: bus_id:%d irq:%d apic_id:%d pin:%d\n",m->bus_bcm5785_0, ((1+sysconf.sbdn)<<2)|1, m->apicid_bcm5785[0], 0xe);
//		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  m->bus_bcm5785_0, (0x02<<2)|1, m->apicid_bcm5785[0], 0xe);

	//onboard Broadcom GbE
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,8, (4<<2)|0, m->apicid_bcm5785[2], 0x4);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,8, (4<<2)|1, m->apicid_bcm5785[2], 0x4);



	/* enable int */
	/* why here? must get the BAR and PCI command bit 1 set before enable it ....*/
	{
		device_t dev;
		dev = dev_find_device(0x1166, 0x0205, 0);
		if(dev) {
			uint32_t dword;
			dword = pci_read_config32(dev, 0x6c);
			dword |= (1<<4); // enable interrupts
			printk(BIOS_DEBUG, "6ch: %x\n",dword);
			pci_write_config32(dev, 0x6c, dword);
		}
	}

/*Local Ints:  Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN#*/
	printk(BIOS_DEBUG, "bus_isa is: %x\n", bus_isa);
	mptable_lintsrc(mc, bus_isa);

	//extended table entries
	smp_write_address_space(mc,0 , ADDRESS_TYPE_IO, 0x0, 0x0, 0x0, 0x0001);
	smp_write_address_space(mc,0 , ADDRESS_TYPE_MEM, 0x0, 0x7f80, 0x0, 0x5e80);
	smp_write_address_space(mc,0 , ADDRESS_TYPE_PREFETCH, 0x0, 0xde00, 0x0, 0x0100);
	smp_write_address_space(mc,0 , ADDRESS_TYPE_MEM, 0x0, 0xdf00, 0x0, 0x1fe0);
	smp_write_address_space(mc,0 , ADDRESS_TYPE_MEM, 0x1000, 0xfee0, 0xf000, 0x011f);
	smp_write_address_space(mc,0 , ADDRESS_TYPE_MEM, 0x0, 0x000a, 0x0, 0x0006);
	smp_write_bus_hierarchy(mc, 9, 0x01, 0);
	smp_write_compatibility_address_space(mc, 0, ADDRESS_RANGE_ADD, 0);
	smp_write_compatibility_address_space(mc, 0, ADDRESS_RANGE_ADD, 1);


	/* Compute the checksums */
	mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);
	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
	printk(BIOS_DEBUG, "Wrote the mp table end at: %p - %p\n",
		mc, smp_next_mpe_entry(mc));
	return smp_next_mpe_entry(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
