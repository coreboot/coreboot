/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * (Written by Yinghai Lu <yinghailu@amd.com> for AMD)
 * Copyright (C) 2007 Philipp Degler <pdegler@rumms.uni-mannheim.de>
 * (Thanks to LSRA University of Mannheim for their support)
 * Copyright (C) 2008 Jonathan A. Kollasch <jakllsch@kollasch.net>
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

extern unsigned char bus_ck804[6];
extern unsigned apicid_ck804;

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int bus_isa;
	unsigned sbdn;

	get_bus_conf();
	sbdn = sysconf.sbdn;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);
	mptable_write_buses(mc, NULL, &bus_isa);

/* I/O APICs:	APIC ID	Version	State		Address*/
	{
		struct device *dev;
		struct resource *res;
		u32 dword;

		dev = dev_find_slot(bus_ck804[0], PCI_DEVFN(sbdn + 0x1, 0));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_1);
			if (res) {
				smp_write_ioapic(mc, apicid_ck804, 0x11,
						 res2mmio(res, 0, 0));
			}

			/* Initialize interrupt mapping */

			/* copied from stock bios */
			/*0x01800500,0x1800d509,0x00520d08*/

			dword = 0x08d0d218;
			pci_write_config32(dev, 0x7c, dword);

			dword = 0x8d001509;
			pci_write_config32(dev, 0x80, dword);

			dword = 0x00010271;
			pci_write_config32(dev, 0x84, dword);

		}
	}

	/* Now, assemble the table. */
	mptable_add_isa_interrupts(mc, bus_isa, apicid_ck804, 0);

#define PCI_INT(bus, dev, fn, pin) \
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, \
		bus_ck804[bus], (((dev)<<2)|(fn)), apicid_ck804, (pin))

#if 0
	// Onboard ck804 smbus
	PCI_INT(0, sbdn+1, 1, 10); /* (this seems odd, how to test?) */

#endif
	// Onboard ck804 USB
	PCI_INT(0, sbdn+2, 0, 23);
	PCI_INT(0, sbdn+2, 1, 23);

	// Onboard ck804 AC-97
	PCI_INT(0, sbdn+4, 0, 23);

	// Onboard ck804 SATA 0
	PCI_INT(0, sbdn+7, 0, 20);

	// Onboard ck804 SATA 1
	PCI_INT(0, sbdn+8, 0, 21);

	// Onboard ck804 NIC
	PCI_INT(0, sbdn+10, 0, 22);


	/* "AGR" slot */
	PCI_INT(1, 0, 0, 16);
	PCI_INT(1, 0, 1, 17);

	/* legacy PCI */
	PCI_INT(1, 7, 0, 17);
	PCI_INT(1, 7, 1, 18);
	PCI_INT(1, 7, 2, 19);
	PCI_INT(1, 7, 3, 16);

	PCI_INT(1, 8, 0, 18);
	PCI_INT(1, 8, 1, 19);
	PCI_INT(1, 8, 2, 16);
	PCI_INT(1, 8, 3, 17);

	PCI_INT(1, 9, 0, 19);
	PCI_INT(1, 9, 1, 16);
	PCI_INT(1, 9, 2, 17);
	PCI_INT(1, 9, 3, 18);


	/* PCI-E x1 port */
	PCI_INT(2, 0, 0, 19);
	/* XXX guesses */
	PCI_INT(2, 0, 1, 16);
	PCI_INT(2, 0, 2, 17);
	PCI_INT(2, 0, 3, 18);

	/* PCI-E x16 port */  /* XXX fix me ? */
	PCI_INT(3, 0, 0, 18);
	/* XXX guesses */
	PCI_INT(3, 0, 1, 19);
	PCI_INT(3, 0, 2, 16);
	PCI_INT(3, 0, 3, 17);

/*Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#*/
	mptable_lintsrc(mc, bus_ck804[0]);

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
