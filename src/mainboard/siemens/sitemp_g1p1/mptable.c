/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <arch/io.h>
#include <string.h>
#include <stdint.h>

#include <cpu/amd/amdk8_sysconf.h>

extern u8 bus_isa;
extern u8 bus_rs690[8];
extern u8 bus_sb600[2];

extern u32 apicid_sb600;

extern u32 bus_type[256];
extern u32 sbdn_rs690;
extern u32 sbdn_sb600;

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int isa_bus;
	
        mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);
	mptable_init(mc, LAPIC_ADDR);
	smp_write_processors(mc);

	get_bus_conf();
	printk(BIOS_DEBUG, "%s: bus_isa=%d, apic_id=0x%x\n", __func__, bus_isa, apicid_sb600);

	mptable_write_buses(mc, NULL, &isa_bus);
	if (isa_bus != bus_isa) {
		printk(BIOS_ERR, "ISA bus numbering schemes differ! Please fix mptable.c\n");
	}
	/* I/O APICs:   APIC ID Version State   Address */
	{
		device_t dev;

		dev = dev_find_slot(bus_sb600[0], PCI_DEVFN(sbdn_sb600 + 0x14, 0));
		if (dev) {
			struct resource *res;
			res = find_resource(dev, 0x74);
			smp_write_ioapic(mc, apicid_sb600, 0x20, res->base);
		}
	}
	mptable_add_isa_interrupts(mc, bus_isa, apicid_sb600, 0);
	/* I/O Ints:    Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
	smp_write_lintsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, MP_APIC_ALL, 0x0);
	smp_write_lintsrc(mc, mp_NMI, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, MP_APIC_ALL, 0x1);

	/* Compute the checksums */
	mc->mpe_checksum =
	    smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);
	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
	printk(BIOS_DEBUG, "Wrote the mp table end at: %p - %p\n",
		     mc, smp_next_mpe_entry(mc));
	return smp_next_mpe_entry(mc);
}

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
