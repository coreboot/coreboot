/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
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
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#include <cpu/amd/amdk8_sysconf.h>

extern unsigned char bus_mcp55[8]; //1

extern unsigned apicid_mcp55;

extern unsigned char bus_pcix[3]; // under bus_mcp55_2

static void *smp_write_config_table(void *v)
{
        struct mp_config_table *mc;
	unsigned sbdn;
	int i, j, bus_isa;

        mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

        smp_write_processors(mc);

	get_bus_conf();
	sbdn = sysconf.sbdn;

	mptable_write_buses(mc, NULL, &bus_isa);

/*I/O APICs:	APIC ID	Version	State		Address*/
        {
                device_t dev;
		struct resource *res;
		uint32_t dword;

                dev = dev_find_slot(bus_mcp55[0], PCI_DEVFN(sbdn+ 0x1,0));
                if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_1);
			if (res) {
				smp_write_ioapic(mc, apicid_mcp55, 0x11, res->base);
			}

			dword = 0x43c6c643;
	        	pci_write_config32(dev, 0x7c, dword);

		        dword = 0x81001a00;
		        pci_write_config32(dev, 0x80, dword);

	        	dword = 0xd00012d2;
		        pci_write_config32(dev, 0x84, dword);

                }



	}

	mptable_add_isa_interrupts(mc, bus_isa, apicid_mcp55, 0);

		   /*I/O Ints:	Type	Polarity    Trigger			Bus ID	 IRQ	APIC ID	PIN# */
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_mcp55[0], ((sbdn+1)<<2)|1, apicid_mcp55, 0xa);

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_mcp55[0], ((sbdn+2)<<2)|0, apicid_mcp55, 0x16); // 22

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_mcp55[0], ((sbdn+2)<<2)|1, apicid_mcp55, 0x17); // 23

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_mcp55[0], ((sbdn+6)<<2)|1, apicid_mcp55, 0x17); // 23

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_mcp55[0], ((sbdn+5)<<2)|0, apicid_mcp55, 0x14); // 20
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_mcp55[0], ((sbdn+5)<<2)|1, apicid_mcp55, 0x17); // 23
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_mcp55[0], ((sbdn+5)<<2)|2, apicid_mcp55, 0x15); // 21

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_mcp55[0], ((sbdn+8)<<2)|0, apicid_mcp55, 0x16); // 22
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_mcp55[0], ((sbdn+9)<<2)|0, apicid_mcp55, 0x15); // 21

	for(j=7; j>=2; j--) {
		if(!bus_mcp55[j]) continue;
	        for(i=0;i<4;i++) {
        	        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_mcp55[j], (0x00<<2)|i, apicid_mcp55, 0x10 + (2+j+i+4-sbdn%4)%4);
        	}
	}

        for(i=0;i<4;i++) {
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_mcp55[1], (0x04<<2)|i, apicid_mcp55, 0x10 + (0+i)%4);
        }


	if(bus_pcix[0]) {
		for(i=0;i<2;i++) {
			smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pcix[2], (4<<2)|i, apicid_mcp55, 0x10 + (0+i+4-sbdn%4)%4); //16, 17
		}

		for(i=0;i<4;i++) {
			smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pcix[1], (4<<2)|i, apicid_mcp55, 0x10 + (2+i+4-sbdn%4)%4); // 18, 19, 16, 17
		}
	}

/*Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#*/
	mptable_lintsrc(mc, bus_isa);
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
