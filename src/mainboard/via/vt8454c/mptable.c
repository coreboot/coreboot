/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <device/device.h>
#include <device/pci.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <string.h>
#include <stdint.h>

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int isa_bus;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LAPIC_ADDR);

	smp_write_processors(mc);
	mptable_write_buses(mc, NULL, &isa_bus);

	/* I/O APICs:   APIC ID Version State Address */
	smp_write_ioapic(mc, 2, 17, IO_APIC_ADDR);

	mptable_add_isa_interrupts(mc, isa_bus, 0x2, 0);

	/* I/O Ints:    Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, 0x0, 0x40, 0x2, 0x14);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, 0x0, 0x41, 0x2, 0x16);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, 0x0, 0x42, 0x2, 0x15);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, 0x0, 0x43, 0x2, 0x17);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, 0x80, 0x4, 0x2, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, 0x1, 0x0, 0x2, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, 0x2, 0x10, 0x2, 0x11);

	/*Local Ints:   Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
	smp_write_lintsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x0, 0x0, MP_APIC_ALL, 0x0);
	smp_write_lintsrc(mc, mp_NMI, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x0, 0x0, MP_APIC_ALL, 0x1);

	/* Compute the checksums */
	mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);
	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
	printk(BIOS_DEBUG, "Wrote the mp table end at: %p - %p\n", mc, smp_next_mpe_entry(mc));
	return smp_next_mpe_entry(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr);
	return (unsigned long)smp_write_config_table(v);
}
