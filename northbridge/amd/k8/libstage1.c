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
/*	This should be done by Eric
	2004.11 yhlu add 4 rank DIMM support
	2004.12 yhlu add D0 support
	2005.02 yhlu add E0 memory hole support
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
#include <mc146818rtc.h>
#include <lib.h>

int cpu_init_detected(unsigned int nodeid)
{
	unsigned long htic;
	u32 dev;

	dev = PCI_BDF(0, 0x18 + nodeid, 0);
	htic = pci_conf1_read_config32(dev, HT_INIT_CONTROL);

	return !!(htic & HTIC_INIT_Detect);
}

int bios_reset_detected(void)
{
	unsigned long htic;
	htic =
	    pci_conf1_read_config32(PCI_BDF(0, 0x18, 0), HT_INIT_CONTROL);

	return (htic & HTIC_ColdR_Detect) && !(htic & HTIC_BIOSR_Detect);
}

int cold_reset_detected(void)
{
	unsigned long htic;
	htic =
	    pci_conf1_read_config32(PCI_BDF(0, 0x18, 0), HT_INIT_CONTROL);

	return !(htic & HTIC_ColdR_Detect);
}

void distinguish_cpu_resets(unsigned int nodeid)
{
	u32 htic;
	u32 device;
	device = PCI_BDF(0, 0x18 + nodeid, 0);
	htic = pci_conf1_read_config32(device, HT_INIT_CONTROL);
	htic |= HTIC_ColdR_Detect | HTIC_BIOSR_Detect | HTIC_INIT_Detect;
	pci_conf1_write_config32(device, HT_INIT_CONTROL, htic);
}

void set_bios_reset(void)
{
	unsigned long htic;
	htic =
	    pci_conf1_read_config32(PCI_BDF(0, 0x18, 0), HT_INIT_CONTROL);
	htic &= ~HTIC_BIOSR_Detect;
	pci_conf1_write_config32(PCI_BDF(0, 0x18, 0), HT_INIT_CONTROL,
				 htic);
}

u8 node_link_to_bus(unsigned int node, unsigned int link)
{
	u16 reg;

	for (reg = 0xE0; reg < 0xF0; reg += 0x04) {
		u32 config_map;
		config_map =
		    pci_conf1_read_config32(PCI_BDF(0, 0x18, 1), reg);
		if ((config_map & 3) != 3) {
			continue;
		}
		if ((((config_map >> 4) & 7) == node) &&
		    (((config_map >> 8) & 3) == link)) {
			return (config_map >> 16) & 0xff;
		}
	}
	return 0;
}

u32 get_sblk(void)
{
	u32 reg;
	/* read PCI_BDF(0,0x18,0) 0x64 bit [8:9] to find out SbLink m */
	reg = pci_conf1_read_config32(PCI_BDF(0, 0x18, 0), 0x64);
	return ((reg >> 8) & 3);
}

u8 get_sbbusn(unsigned int sblk)
{
	return node_link_to_bus(0, sblk);
}


/* this supports early enumeration of the ht chain */
/*  
	2005.11 yhlu add let the real sb to use small unitid
*/
// only for sb ht chain
void enumerate_ht_chain(void)
{
#if HT_CHAIN_UNITID_BASE != 0
/*  HT_CHAIN_UNITID_BASE could be 0 (only one ht device in the ht chain), if so, don't need to go through the chain  */

	/* Assumption the HT chain that is bus 0 has the HT I/O Hub on it.
	 * On most boards this just happens.  If a cpu has multiple
	 * non Coherent links the appropriate bus registers for the
	 * links needs to be programed to point at bus 0.
	 */
	unsigned next_unitid, last_unitid;
	u32 dev;
#if  HT_CHAIN_END_UNITID_BASE != 0x20
	//let's record the device of last ht device, So we can set the Unitid to  HT_CHAIN_END_UNITID_BASE
	unsigned real_last_unitid = 0;
	u8 real_last_pos = 0;
	int ht_dev_num = 0;	// except host_bridge
	u8 end_used = 0;
#endif

	dev = PCI_BDF(0, 0, 0);
	next_unitid =  HT_CHAIN_UNITID_BASE;
	do {
		u32 id;
		u8 hdr_type, pos;
		printk(BIOS_SPEW, "dev 0x%x unitid %d: ", dev, next_unitid);
		last_unitid = next_unitid;

		id = pci_conf1_read_config32(dev, PCI_VENDOR_ID);
		printk(BIOS_SPEW, "id 0x%x\n", id);
		/* If the chain is enumerated quit */
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff)
		    || (((id >> 16) & 0xffff) == 0xffff)
		    || (((id >> 16) & 0xffff) == 0x0000)) {
			break;
		}

		hdr_type = pci_conf1_read_config8(dev, PCI_HEADER_TYPE);
		pos = 0;
		hdr_type &= 0x7f;

		if ((hdr_type == PCI_HEADER_TYPE_NORMAL) ||
		    (hdr_type == PCI_HEADER_TYPE_BRIDGE)) {
			pos =
			    pci_conf1_read_config8(dev,
						   PCI_CAPABILITY_LIST);
		}
		while (pos != 0) {
			u8 cap;
			cap =
			    pci_conf1_read_config8(dev,
						   pos + PCI_CAP_LIST_ID);
			if (cap == PCI_CAP_ID_HT) {
				u16 flags;
				printk(BIOS_SPEW, "Found CAP HT\n");
				/* Read and write and reread flags so the link
				 * direction bit is valid.
				 */
				flags =
				    pci_conf1_read_config16(dev,
							    pos +
							    PCI_CAP_FLAGS);
				pci_conf1_write_config16(dev,
							 pos +
							 PCI_CAP_FLAGS,
							 flags);
				flags =
				    pci_conf1_read_config16(dev,
							    pos +
							    PCI_CAP_FLAGS);
				if ((flags >> 13) == 0) {
					unsigned count;
					unsigned ctrl, ctrl_off;
					u32 devx;

#if  HT_CHAIN_END_UNITID_BASE != 0x20
					if (next_unitid >= 0x18) {	// don't get mask out by k8, at this time BSP, RT is not enabled, it will response from 0x18,0--0x1f.
						if (!end_used) {
							next_unitid =
							     HT_CHAIN_END_UNITID_BASE;
							end_used = 1;
						} else {
							goto out;
						}
					}
					real_last_unitid = next_unitid;
					real_last_pos = pos;
					ht_dev_num++;
#endif

					flags &= ~0x1f;
					flags |= next_unitid & 0x1f;
					count = (flags >> 5) & 0x1f;

					devx = PCI_BDF(0, next_unitid, 0);
					pci_conf1_write_config16(dev,
								 pos +
								 PCI_CAP_FLAGS,
								 flags);

					printk(BIOS_SPEW, "devx 0x%x\n", devx);
					next_unitid += count;

					flags =
					    pci_conf1_read_config16(devx,
								    pos +
								    PCI_CAP_FLAGS);
					/* Test for end of chain */
					ctrl_off = ((flags >> 10) & 1) ? PCI_HT_CAP_SLAVE_CTRL0 : PCI_HT_CAP_SLAVE_CTRL1;	// another end

					do {
						ctrl =
						    pci_conf1_read_config16(devx,
								      pos +
								      ctrl_off);
						/* Is this the end of the hypertransport chain? */
						if (ctrl & (1 << 6)) {
							goto out;
						}

						if (ctrl &
						    ((1 << 4) | (1 << 8)))
						{
							/*
							 * Either the link has failed, or we have
							 * a CRC error.
							 * Sometimes this can happen due to link
							 * retrain, so lets knock it down and see
							 * if its transient
							 */
							ctrl |= ((1 << 4) | (1 << 8));	// Link fail + Crc
							pci_conf1_write_config16
							    (devx,
							     pos +
							     ctrl_off,
							     ctrl);
							ctrl =
							    pci_conf1_read_config16
							    (devx,
							     pos +
							     ctrl_off);
							if (ctrl &
							    ((1 << 4) |
							     (1 << 8))) {
								// can not clear the error
								break;
							}
						}
					} while ((ctrl & (1 << 5)) == 0);

					break;
				}
			}
			pos =
			    pci_conf1_read_config8(dev,
						   pos +
						   PCI_CAP_LIST_NEXT);
		}
	} while (last_unitid != next_unitid);

      out:
	;

#if  HT_CHAIN_END_UNITID_BASE != 0x20
	if ((ht_dev_num > 1)
	    && (real_last_unitid !=  HT_CHAIN_END_UNITID_BASE)
	    && !end_used) {
		u16 flags;
		dev = PCI_BDF(0, real_last_unitid, 0);
		flags =
		    pci_conf1_read_config16(dev,
					    real_last_pos + PCI_CAP_FLAGS);
		flags &= ~0x1f;
		flags |=  HT_CHAIN_END_UNITID_BASE & 0x1f;
		pci_conf1_write_config16(dev,
					 real_last_pos + PCI_CAP_FLAGS,
					 flags);
	}
#endif

	printk(BIOS_SPEW, "Done: ht_dev_num %d, real_last_pos %d\n",
			ht_dev_num, real_last_pos);
#endif

}
