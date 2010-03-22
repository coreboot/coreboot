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
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <string.h>
#include <stdint.h>

static void *smp_write_config_table(void *v)
{
	static const char sig[4] = MPC_SIGNATURE;
	static const char oem[8] = "COREBOOT";
	static const char productid[12] = "VIA VT8454C ";
	struct mp_config_table *mc;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);
	memset(mc, 0, sizeof(*mc));

	memcpy(mc->mpc_signature, sig, sizeof(sig));
	mc->mpc_length = sizeof(*mc);	/* initially just the header */
	mc->mpc_spec = 0x04;
	mc->mpc_checksum = 0;	/* not yet computed */
	memcpy(mc->mpc_oem, oem, sizeof(oem));
	memcpy(mc->mpc_productid, productid, sizeof(productid));
	mc->mpc_oemptr = 0;
	mc->mpc_oemsize = 0;
	mc->mpc_entry_count = 0;	/* No entries yet... */
	mc->mpc_lapic = LAPIC_ADDR;
	mc->mpe_length = 0;
	mc->mpe_checksum = 0;
	mc->reserved = 0;

	smp_write_processors(mc);

	/*Bus:          Bus ID  Type */
	smp_write_bus(mc, 0, "PCI   ");
	smp_write_bus(mc, 1, "PCI   ");
	smp_write_bus(mc, 2, "PCI   ");
	smp_write_bus(mc, 128, "PCI   ");
	smp_write_bus(mc, 129, "ISA   ");

	/* I/O APICs:   APIC ID Version State Address */
	smp_write_ioapic(mc, 2, 17, 0xfec00000);

	/* I/O Ints:    Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, 0x0, 0x40, 0x2, 0x14);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, 0x0, 0x41, 0x2, 0x16);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, 0x0, 0x42, 0x2, 0x15);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, 0x0, 0x43, 0x2, 0x17);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, 0x80, 0x4, 0x2, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, 0x1, 0x0, 0x2, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, 0x2, 0x10, 0x2, 0x11);
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x81, 0x0, 0x2, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x81, 0x1, 0x2, 0x1);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x81, 0x0, 0x2, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x81, 0x3, 0x2, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x81, 0x4, 0x2, 0x4);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x81, 0x6, 0x2, 0x6);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x81, 0x7, 0x2, 0x7);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, 0x81, 0x8, 0x2, 0x8);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x81, 0x9, 0x2, 0x9);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x81, 0xc, 0x2, 0xc);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x81, 0xd, 0x2, 0xd);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x81, 0xe, 0x2, 0xe);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x81, 0xf, 0x2, 0xf);

	/*Local Ints:   Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x0, 0x0, MP_APIC_ALL, 0x0);
	smp_write_intsrc(mc, mp_NMI, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, 0x0, 0x0, MP_APIC_ALL, 0x1);

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
