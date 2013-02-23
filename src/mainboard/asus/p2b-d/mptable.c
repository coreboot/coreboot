/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
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

static void *smp_write_config_table(void *v)
{
	int ioapic_id, ioapic_ver, isa_bus;
	struct mp_config_table *mc;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	mptable_write_buses(mc, NULL, &isa_bus);

	ioapic_id = 2;
	ioapic_ver = 0x11; /* External Intel 82093AA IOAPIC. */
	smp_write_ioapic(mc, ioapic_id, ioapic_ver, IO_APIC_ADDR);

	/* Legacy Interrupts */
	mptable_add_isa_interrupts(mc, isa_bus, ioapic_id, 0);

	/* I/O Ints:         Type       Trigger                Polarity              Bus ID   IRQ   APIC ID      PIN# */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW,  0x0,     0x13, ioapic_id,   0x13); /* UHCI */

	/* Local Ints:       Type       Trigger                Polarity              Bus ID   IRQ   APIC ID      PIN# */
	mptable_lintsrc(mc, isa_bus);

	/* Compute the checksums. */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
