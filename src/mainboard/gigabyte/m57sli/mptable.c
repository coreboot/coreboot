/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
 * Copyright (C) 2009 Harald Gutmann <harald.gutmann@gmx.net>
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

extern unsigned char bus_mcp55[8]; //1

extern unsigned apicid_mcp55;

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	unsigned sbdn;
	int i, j, k, bus_isa;

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

		dev = dev_find_slot(bus_mcp55[0], PCI_DEVFN(sbdn+ 0x1,0));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_1);
			if (res) {
				smp_write_ioapic(mc, apicid_mcp55, 0x11,
						 res2mmio(res, 0, 0));
			}
			/* set up the interrupt registers of mcp55 */
			pci_write_config32(dev, 0x7c, 0xc643c643);
			pci_write_config32(dev, 0x80, 0x8da01009);
			pci_write_config32(dev, 0x84, 0x200018d2);
		}
	}

	mptable_add_isa_interrupts(mc, bus_isa, apicid_mcp55, 0);

/* PCI interrupts are level triggered, and are
 * associated with a specific bus/device/function tuple.
 */
#define PCI_INT(bus, dev, fn, pin)					\
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,\
			 bus_mcp55[bus], (((dev)<<2)|(fn)), apicid_mcp55, (pin))

	PCI_INT(0,sbdn+1,1, 10); /* SMBus */
	PCI_INT(0,sbdn+2,0, 22); /* USB */
	PCI_INT(0,sbdn+2,1, 23); /* USB */
	PCI_INT(0,sbdn+4,0, 21); /* IDE */
	PCI_INT(0,sbdn+5,0, 20); /* SATA */
	PCI_INT(0,sbdn+5,1, 21); /* SATA */
	PCI_INT(0,sbdn+5,2, 22); /* SATA */
	PCI_INT(0,sbdn+6,1, 23); /* HD Audio */
	PCI_INT(0,sbdn+8,0, 20); /* GBit Ethernet */

	/* The PCIe slots, each on its own bus */
	k = 1;
	for(i = 0; i < 4; i++){
		for(j = 7; j > 1; j--){
			if(k > 3) k = 0;
			PCI_INT(j,0,i, 16+k);
			k++;
		}
		k--;
	}

	/* On bus 1: the PCI bus slots...
	 * physical PCI slots are j = 7,8
	 * FireWire is j = 10
	*/
	k = 2;
	for(i = 0; i < 4; i++){
		for(j = 6; j < 11; j++){
			if(k > 3) k = 0;
			PCI_INT(1,j,i, 16+k);
			k++;
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
