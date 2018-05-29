/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * (Written by Yinghai Lu <yinghailu@amd.com> for AMD)
 * Copyright (C) 2007 Philipp Degler <pdegler@rumms.uni-mannheim.de>
 * (Thanks to LSRA University of Mannheim for their support)
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
	unsigned sbdn;
	int bus_isa;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	get_bus_conf();
	sbdn = sysconf.sbdn;

	mptable_write_buses(mc, NULL, &bus_isa);

	/* I/O APICs:	APIC ID	Version	State		Address */
	{
		struct device *dev;
		struct resource *res;
		uint32_t dword;

		dev = dev_find_slot(bus_ck804[0], PCI_DEVFN(sbdn + 0x1, 0));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_1);
			if (res) {
				smp_write_ioapic(mc, apicid_ck804, 0x11,
						 res2mmio(res, 0, 0));
			}

			/* Initialize interrupt mapping. */
			dword = 0x01200000;
			pci_write_config32(dev, 0x7c, dword);

			dword = 0x12008009;
			pci_write_config32(dev, 0x80, dword);

			dword = 0x0002010d;
			pci_write_config32(dev, 0x84, dword);

		}
	}

	mptable_add_isa_interrupts(mc, bus_isa, apicid_ck804, 0);

	// Onboard ck804 smbus
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW,
			 bus_ck804[0], ((sbdn + 1) << 2) | 1, apicid_ck804,
			 0xa);

	// Onboard ck804 USB 1.1
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW,
			 bus_ck804[0], ((sbdn + 2) << 2) | 0, apicid_ck804,
			 0x15);

	// Onboard ck804 USB 2
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW,
			 bus_ck804[0], ((sbdn + 2) << 2) | 1, apicid_ck804,
			 0x14);

	/* Onboard audio  */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW,
			 bus_ck804[0], ((sbdn + 4) << 2) | 0, apicid_ck804, 0x3);

	// Onboard ck804 SATA 0
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW,
			 bus_ck804[0], ((sbdn + 7) << 2) | 0, apicid_ck804,
			 0x17);

	// Onboard ck804 SATA 1
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW,
			 bus_ck804[0], ((sbdn + 8) << 2) | 0, apicid_ck804,
			 0x16);

	// Onboard ck804 NIC
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW,
			 bus_ck804[0], ((sbdn + 10) << 2) | 0, apicid_ck804,
			 0x17);

	/* Local Ints: Type Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN# */
	mptable_lintsrc(mc, bus_ck804[0]);

	/* There is no extension information... */

	/* Compute the checksums. */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
