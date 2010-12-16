/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <string.h>
#include <stdint.h>

static void *smp_write_config_table(void *v)
{
        struct mp_config_table *mc;
	struct device *riser = NULL, *firewire = NULL;
	int firewire_bus = 0, riser_bus = 0, isa_bus;
	int ioapic_id;

        mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LAPIC_ADDR);

        smp_write_processors(mc);

	firewire = dev_find_device(0x104c, 0x8023, 0);
	if (firewire) {
		firewire_bus = firewire->bus->secondary;
	}

	// If a riser card is used, this riser is detected on bus 4, so its secondary bus is the
	// highest bus number on the pci bus.
	riser = dev_find_device(0x3388, 0x0021, 0);
	if (!riser)
		riser = dev_find_device(0x3388, 0x0022, 0);
	if (riser) {
		riser_bus = riser->link_list->secondary;
	}

	mptable_write_buses(mc, NULL, &isa_bus);

	/* I/O APICs:	APIC ID	Version	State		Address */
	ioapic_id = 2;
	smp_write_ioapic(mc, ioapic_id, 0x20, IO_APIC_ADDR);

	/* Legacy Interrupts */
	mptable_add_isa_interrupts(mc, isa_bus, ioapic_id, 0);

	/* Builtin devices on Bus 0 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x4, ioapic_id, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x8, ioapic_id, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x7d, ioapic_id, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x74, ioapic_id, 0x17);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x75, ioapic_id, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x76, ioapic_id, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x77, ioapic_id, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x6c, ioapic_id, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x70, ioapic_id, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x71, ioapic_id, 0x11);

	/* Internal PCI bus (Firewire, PCI slot) */
	if (firewire) {
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, firewire_bus, 0x0, ioapic_id, 0x10);
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, firewire_bus, 0x4, ioapic_id, 0x14);
	}

	if (riser) {
		/* Old riser card */
		// riser slot top 5:8.0
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, riser_bus, 0x20, ioapic_id, 0x14);
		// riser slot middle 5:9.0
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, riser_bus, 0x24, ioapic_id, 0x15);
		// riser slot bottom 5:a.0
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, riser_bus, 0x28, ioapic_id, 0x16);

		/* New Riser Card */
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, riser_bus, 0x30, ioapic_id, 0x14);
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, riser_bus, 0x34, ioapic_id, 0x15);
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, riser_bus, 0x38, ioapic_id, 0x16);
	}

	/* PCIe slot */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x1, 0x0, ioapic_id, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x1, 0x1, ioapic_id, 0x11);

	/* Onboard Ethernet */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x2, 0x0, ioapic_id, 0x10);

	/* Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN# */
	smp_write_lintsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, isa_bus, 0x0, MP_APIC_ALL, 0x0);
	smp_write_lintsrc(mc, mp_NMI, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, isa_bus, 0x0, MP_APIC_ALL, 0x1);

	/* Compute the checksums */
	mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);
	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);

	printk(BIOS_DEBUG, "Wrote the mp table end at: %p - %p\n", mc, smp_next_mpe_entry(mc));

	return smp_next_mpe_entry(mc);
}

/* MP table generation in coreboot is not very well designed;
 * One of the issues is that it knows nothing about Virtual
 * Wire mode, which everyone uses since a decade or so. This
 * function fixes up our floating table. This spares us doing
 * a half-baked fix of adding a new parameter to 200+ calls
 * to smp_write_floating_table()
 */
static void fixup_virtual_wire(void *v)
{
        struct intel_mp_floating *mf = v;

        mf->mpf_checksum = 0;
        mf->mpf_feature2 = MP_FEATURE_VIRTUALWIRE;
        mf->mpf_checksum = smp_compute_checksum(mf, mf->mpf_length*16);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr);
	fixup_virtual_wire(v);
	return (unsigned long)smp_write_config_table(v);
}
