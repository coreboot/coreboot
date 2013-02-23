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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#include "southbridge/via/vt8237r/vt8237r.h"

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int isa_bus;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);
	mptable_write_buses(mc, NULL, &isa_bus);

/* I/O APICs:	APIC ID	Version	State		Address*/
	smp_write_ioapic(mc, VT8237R_APIC_ID, 0x20, IO_APIC_ADDR);

	/* Now, assemble the table. */
	mptable_add_isa_interrupts(mc, isa_bus, VT8237R_APIC_ID, 0);

#define PCI_INT(bus, dev, fn, pin) \
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, \
		bus, (((dev)<<2)|(fn)), VT8237R_APIC_ID, (pin))

	// PCI slot 1
	PCI_INT(0, 8, 0, 16);
	PCI_INT(0, 8, 1, 17);
	PCI_INT(0, 8, 2, 18);
	PCI_INT(0, 8, 3, 19);

	// PCI slot 2
	PCI_INT(0, 9, 0, 17);
	PCI_INT(0, 9, 1, 18);
	PCI_INT(0, 9, 2, 19);
	PCI_INT(0, 9, 3, 16);

	// SATA
	PCI_INT(0, 15, 1, 20);

	// USB
	PCI_INT(0, 16, 0, 21);
	PCI_INT(0, 16, 1, 21);
	PCI_INT(0, 16, 2, 21);
	PCI_INT(0, 16, 3, 21);

	// Audio
	PCI_INT(0, 17, 2, 22);

	// Ethernet
	PCI_INT(0, 18, 0, 23);

	/* Onboard VGA */
	PCI_INT(1, 0, 0, 16);

/*Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#*/
	mptable_lintsrc(mc, 0);

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
