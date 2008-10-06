/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2002 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2004 YingHai Lu
 * Copyright (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */
/*
 	This should be done by Eric
	2004.12 yhlu add multi ht chain dynamically support
	2005.11 yhlu add let real sb to use small unitid
*/
#include <mainboard.h>
#include <console.h>
#include <mtrr.h>
#include <macros.h>
#include <spd.h>
#include <cpu.h>
#include <msr.h>
#include <amd/k8/k8.h>
#include <amd/k8/sysconf.h>
#include <device/pci.h>
#include <device/hypertransport_def.h>
#include <mc146818rtc.h>
#include <lib.h>

int set_ht_link_buffer_count(u8 node, u8 linkn, u8 linkt, unsigned val)
{
        u32 dword;
        u8 link_type;
	unsigned regpos;
	u32 bdf;

        /* This works on an Athlon64 because unimplemented links return 0 */
	regpos = 0x98 + (linkn * 0x20);
	bdf = PCI_BDF(0,0x18+node,0);
        dword = pci_conf1_read_config32(bdf, regpos);
        link_type = dword & 0xff;

        if ( (link_type & 0x7) == linkt ) { /* Coherent Link only linkt = 3, ncoherent = 7*/
		regpos = 0x90 + (linkn * 0x20);
        	dword = pci_conf1_read_config32(bdf, regpos );

	        if (dword != val) {
        	        pci_conf1_write_config32(bdf, regpos, val);
	                return 1;
        	}
	}

        return 0;
}

int set_ht_link_buffer_counts_chain(u8 ht_c_num, unsigned vendorid,  unsigned val)
{
        int reset_needed;
        u8 i;

        reset_needed = 0;

        for (i = 0; i < ht_c_num; i++) {
                u32 reg;
                u8 nodeid, linkn;
                u8 busn;
                unsigned devn;

                reg = pci_conf1_read_config32(PCI_BDF(0,0x18,1), 0xe0 + i * 4);
                if((reg & 3) != 3) continue; // not enabled

                nodeid = ((reg & 0xf0)>>4); // nodeid
                linkn = ((reg & 0xf00)>>8); // link n
                busn = (reg & 0xff0000)>>16; //busn

		for(devn = 0; devn < 0x20; devn++) {
	                reg = pci_conf1_read_config32( PCI_BDF(busn, devn, 0), PCI_VENDOR_ID); //1?
        	        if ( (reg & 0xffff) == vendorid ) {
                	        reset_needed |= set_ht_link_buffer_count(nodeid, linkn, 0x07,val);
				break;
                	}
		}
        }

        return reset_needed;
}

