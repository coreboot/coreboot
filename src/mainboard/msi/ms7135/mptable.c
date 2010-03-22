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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

#include <cpu/amd/amdk8_sysconf.h>

extern unsigned char bus_isa;
extern unsigned char bus_ck804[6];
extern unsigned apicid_ck804;

extern unsigned bus_type[256];

extern void get_bus_conf(void);

static void *smp_write_config_table(void *v)
{
	static const char sig[4] = "PCMP";
	static const char oem[8] = "MSI     ";
	static const char productid[12] = "MS7135      ";
	struct mp_config_table *mc;
	unsigned sbdn;

	int bus_num;
	int i;

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

	get_bus_conf();
	sbdn = sysconf.sbdn;

/* Bus:		Bus ID	Type*/
	/* define numbers for pci and isa bus */
	for (bus_num = 0; bus_num < 256; bus_num++) {
		if (bus_type[bus_num])
			smp_write_bus(mc, bus_num, "PCI   ");
	}
	smp_write_bus(mc, bus_isa, "ISA   ");


/* I/O APICs:	APIC ID	Version	State		Address*/
	{
		device_t dev;
		struct resource *res;
		uint32_t dword;

		dev = dev_find_slot(bus_ck804[0], PCI_DEVFN(sbdn + 0x1, 0));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_1);
			if (res) {
				smp_write_ioapic(mc, apicid_ck804, 0x11,
						 res->base);
			}

			/* Initialize interrupt mapping */

			/* copied from stock bios */
			/*0x01800500,0x1800d509,0x00520d08*/

			/* if this register is what i think it is ... */
			dword = 0x08d0d218;
			pci_write_config32(dev, 0x7c, dword);

			dword = 0x8d001509;
			pci_write_config32(dev, 0x80, dword);

			dword = 0x00010271;
			pci_write_config32(dev, 0x84, dword);

		}
	}

	/* Now, assemble the table. */

	smp_write_intsrc(mc, mp_ExtINT,
			 MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
			 bus_isa, 0x0, apicid_ck804, 0x0);

#define ISA_INT(intr, pin) \
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, \
		bus_isa, (intr), apicid_ck804, (pin))

	ISA_INT(1, 1);
	ISA_INT(0, 2);
	ISA_INT(3, 3);
	ISA_INT(4, 4);

	ISA_INT(6, 6);
	ISA_INT(7, 7);
	ISA_INT(8, 8);
	ISA_INT(9, 9);

	ISA_INT(0xc, 0xc);
	ISA_INT(0xd, 0xd);
	ISA_INT(0xe, 0xe);
	ISA_INT(0xf, 0xf);

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
	smp_write_lintsrc(mc, mp_ExtINT,
			 MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT,
			 bus_ck804[0], 0x0, MP_APIC_ALL, 0x0);
	smp_write_lintsrc(mc, mp_NMI,
			 MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT,
			 bus_ck804[0], 0x0, MP_APIC_ALL, 0x1);

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
