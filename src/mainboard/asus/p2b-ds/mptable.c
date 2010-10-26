/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
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
#include <arch/ioapic.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, "P2B-DS      ", LAPIC_ADDR);

	smp_write_processors(mc);

	/* Bus:		Bus ID	Type */
	smp_write_bus(mc, 0, "PCI   ");
	smp_write_bus(mc, 1, "ISA   ");

	/* I/O APICs:	APIC ID	Version	State		Address */
	smp_write_ioapic(mc, 2, 0x20, IO_APIC_ADDR);
	{
		device_t dev;
		struct resource *res;

		dev = dev_find_slot(1, PCI_DEVFN(0x1e, 0));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res)
				smp_write_ioapic(mc, 3, 0x20, res->base);
		}
		dev = dev_find_slot(1, PCI_DEVFN(0x1c, 0));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res)
				smp_write_ioapic(mc, 4, 0x20, res->base);
		}
		dev = dev_find_slot(4, PCI_DEVFN(0x1e, 0));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res)
				smp_write_ioapic(mc, 5, 0x20, res->base);
		}
		dev = dev_find_slot(4, PCI_DEVFN(0x1c, 0));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res)
				smp_write_ioapic(mc, 8, 0x20, res->base);
		}
	}

	mptable_add_isa_interrupts(mc, 0x1, 0x2, 0);

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW,
			 0x0, 0x13, 0x2, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW,
			 0x0, 0x18, 0x2, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW,
			 0x0, 0x30, 0x2, 0x10);

	/* Local Ints: Type  Polarity  Trigger  Bus ID  IRQ  APIC ID  PIN# */
	smp_write_lintsrc(mc, mp_ExtINT,
			 MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, 0x1, 0x0,
			 MP_APIC_ALL, 0x0);
	smp_write_lintsrc(mc, mp_NMI, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
			 0x1, 0x0, MP_APIC_ALL, 0x1);

	/* There is no extension information... */

	/* Compute the checksums */
	mc->mpe_checksum =
	    smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);
	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
	printk(BIOS_DEBUG, "Wrote the mp table end at: %p - %p\n",
		     mc, smp_next_mpe_entry(mc));
	return smp_next_mpe_entry(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr);
	return (unsigned long)smp_write_config_table(v);
}
