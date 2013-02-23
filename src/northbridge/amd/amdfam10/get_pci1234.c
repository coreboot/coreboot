/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>

#include <cpu/amd/amdfam10_sysconf.h>


/* Need pci1234 array
 * pci1234[0] will record sblink and bus range
 * pci1234[i] will record ht chain i.
 * It will keep the sequence when some ht io card is not installed.
 *
 *	1n: 8
 *	2n: 7x2
 *	3n: 6x3
 *	4n: 5x4
 *	5n: 4x5
 *	6n: 3x6
 *	7n: 2x7
 *	8n: 1x8
 *
 *	8n(4x2): 8x4
 *	16n(4x4): 16*2
 *	20n(4x5): 20x1
 *	32n(4x4+4x4): 16x1
 *
 * Total: xxx: I just want to use 32 instead, If you have more, you may need to
 * reset HC_POSSIBLE_NUM and update ssdt.dsl (hcdn, hclk)
 *
 * Put all the possible ht node/link to the list tp pci1234[] in  get_bus_conf.c
 * on MB dir. Also, don't forget to increase the CONFIG_ACPI_SSDTX_NUM etc if you have
 * too much SSDT. How about co-processor on socket 1 on 2 way system.
 * or socket 2, and socket3 on 4 way system? treat that as one hc too!
 *
 */

#include "northbridge.h"

void get_pci1234(void)
{

	int i,j;
	u32 dword;

	dword = sysconf.sblk<<8;
	dword |= 1;
	sysconf.pci1234[0] = dword; // sblink
	sysconf.hcid[0] = 0;

	/* about hardcode numbering for HT_IO support
	  set the node_id and link_id that could have ht chain in the one array,
	  then check if is enabled.... then update final value
	*/

	//here we need to set hcdn
	//1. hypertransport.c need to record hcdn_reg together with 0xe0, 0xe4, 0xe8, 0xec when are set
	//2. so at the same time we need update hsdn with hcdn_reg here
//	printk(BIOS_DEBUG, "sysconf.ht_c_num = %02d\n", sysconf.ht_c_num);

	for(j=0;j<sysconf.ht_c_num;j++) {
		u32 dwordx;
		dwordx = sysconf.ht_c_conf_bus[j];
//		printk(BIOS_DEBUG, "sysconf.ht_c_conf_bus[%02d] = %08x\n", j, sysconf.ht_c_conf_bus[j]);
		dwordx &=0xfffffffd; //keep bus num, node_id, link_num, enable bits
		if((dwordx & 0x7fd) == dword) { //SBLINK
			sysconf.pci1234[0] = dwordx;
			sysconf.hcdn[0] = sysconf.hcdn_reg[j];
			continue;
		}
		if((dwordx & 1)) {
			// We need to find out the number of HC
			// for exact match
			for(i=1;i<sysconf.hc_possible_num;i++) {
				if((dwordx & 0x7fc) == (sysconf.pci1234[i] & 0x7fc)) { // same node and same linkn
					sysconf.pci1234[i] = dwordx;
					sysconf.hcdn[i] = sysconf.hcdn_reg[j];
					break;
				}
			}
			// for 0xffc match or same node
			for(i=1;i<sysconf.hc_possible_num;i++) {
				if((dwordx & 0x7fc) == (dwordx & sysconf.pci1234[i] & 0x7fc)) {
					sysconf.pci1234[i] = dwordx;
					sysconf.hcdn[i] = sysconf.hcdn_reg[j];
					break;
				}
			}
		}
	}

	for(i=1;i<sysconf.hc_possible_num;i++) {
		if(!(sysconf.pci1234[i] & 1)) {
			sysconf.pci1234[i] = 0;
			sysconf.hcdn[i] = 0x20202020;
		}
		sysconf.hcid[i] = 0;
	}
}
