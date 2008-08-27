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
static void print_linkn_in (const char *strval, u8 byteval)
{
	printk(BIOS_DEBUG, "%s%02x\n", strval, byteval); 
}

u8 ht_lookup_capability(u32 bdf, u16 val)
{
	u8 pos;
	u8 hdr_type;

	hdr_type = pci_conf1_read_config8(bdf, PCI_HEADER_TYPE);
	pos = 0;
	hdr_type &= 0x7f;

	if ((hdr_type == PCI_HEADER_TYPE_NORMAL) ||
	    (hdr_type == PCI_HEADER_TYPE_BRIDGE)) {
		pos = PCI_CAPABILITY_LIST;
	}
	if (pos > PCI_CAP_LIST_NEXT) {
		pos = pci_conf1_read_config8(bdf, pos);
	}
	while(pos != 0) { /* loop through the linked list */
		u8 cap;
		cap = pci_conf1_read_config8(bdf, pos + PCI_CAP_LIST_ID);
		if (cap == PCI_CAP_ID_HT) {
			u16 flags;

			flags = pci_conf1_read_config16(bdf, pos + PCI_CAP_FLAGS);
			if ((flags >> 13) == val) {
				/* Entry is a slave or host , success... */
				break;
			}
		}
		pos = pci_conf1_read_config8(bdf, pos + PCI_CAP_LIST_NEXT);
	}
	return pos;
}

static u8 ht_lookup_slave_capability(u32 bdf)
{          
	return ht_lookup_capability(bdf, 0); // Slave/Primary Interface Block Format
}

static u8 ht_lookup_host_capability(u32 bdf)
{
        return ht_lookup_capability(bdf, 1); // Host/Secondary Interface Block Format
}

static void ht_collapse_previous_enumeration(u8 bus, unsigned offset_unitid)
{
	u32 bdf;
	u32 id;

	//actually, only for one HT device HT chain, and unitid is 0
#if HT_CHAIN_UNITID_BASE == 0
	if(offset_unitid) {
		return;
	}
#endif

	/* Check if is already collapsed */
	if((!offset_unitid) || (offset_unitid && (!((HT_CHAIN_END_UNITID_BASE == 0) && (HT_CHAIN_END_UNITID_BASE <HT_CHAIN_UNITID_BASE))))) {
		bdf = PCI_BDF(bus, 0, 0);
        	id = pci_conf1_read_config32(bdf, PCI_VENDOR_ID);
	        if ( ! ( (id == 0xffffffff) || (id == 0x00000000) ||
        	    (id == 0x0000ffff) || (id == 0xffff0000) ) ) {
	                     return;
        	}
	} 

	/* Spin through the devices and collapse any previous
	 * hypertransport enumeration.
	 */
	for(bdf = PCI_BDF(bus, 1, 0); bdf <= PCI_BDF(bus, 0x1f, 0x7); bdf += PCI_BDF(0, 1, 0)) {
		u32 id;
		u8 pos;
		u16 flags;
		
		id = pci_conf1_read_config32(bdf, PCI_VENDOR_ID);
		if ((id == 0xffffffff) || (id == 0x00000000) ||
		    (id == 0x0000ffff) || (id == 0xffff0000)) {
			continue;
		}
		
		pos = ht_lookup_slave_capability(bdf);
		if (!pos) {
			continue;
		}

		/* Clear the unitid */
		flags = pci_conf1_read_config16(bdf, pos + PCI_CAP_FLAGS);
		flags &= ~0x1f;
		pci_conf1_write_config16(bdf, pos + PCI_CAP_FLAGS, flags);
	}
}

static u16 ht_read_freq_cap(u32 bdf, u8 pos)
{
	/* Handle bugs in valid hypertransport frequency reporting */
	u16 freq_cap;
	u32 id;

	freq_cap = pci_conf1_read_config16(bdf, pos);
	freq_cap &= ~(1 << HT_FREQ_VENDOR); /* Ignore Vendor HT frequencies */

	id = pci_conf1_read_config32(bdf, 0);

	/* AMD 8131 Errata 48 */
	if (id == (PCI_VENDOR_ID_AMD | (PCI_DEVICE_ID_AMD_8131_PCIX << 16))) {
		freq_cap &= ~(1 << HT_FREQ_800Mhz);
		return freq_cap;
	}

	/* AMD 8151 Errata 23 */
	if (id == (PCI_VENDOR_ID_AMD | (PCI_DEVICE_ID_AMD_8151_SYSCTRL << 16))) {
		freq_cap &= ~(1 << HT_FREQ_800Mhz);
		return freq_cap;
	} 
	
	/* AMD K8 Unsupported 1Ghz? */
	if (id == (PCI_VENDOR_ID_AMD | (0x1100 << 16))) {
	#ifndef K8_HT_FREQ_1G_SUPPORT	
                freq_cap &= ~(1 << HT_FREQ_1000Mhz);
	#endif
	}

	return freq_cap;
}
static u8 ht_read_width_cap(u32 bdf, u8 pos)
{
	u8 width_cap = pci_conf1_read_config8(bdf, pos);

	u32 id;

	id = pci_conf1_read_config32(bdf, 0);

	/* netlogic micro cap doesn't support 16 bit yet */
	if (id == (0x184e | (0x0001 << 16))) {
		if((width_cap & 0x77) == 0x11) {
			width_cap &= 0x88;
		}
	}
	
	return width_cap;
	
}
#define LINK_OFFS(CTRL, WIDTH,FREQ,FREQ_CAP) \
      (((CTRL & 0xff) << 24) | ((WIDTH & 0xff) << 16) | ((FREQ & 0xff) << 8) | (FREQ_CAP & 0xFF))

#define LINK_CTRL(OFFS)     ((OFFS >> 24) & 0xFF)
#define LINK_WIDTH(OFFS)    ((OFFS >> 16) & 0xFF)
#define LINK_FREQ(OFFS)     ((OFFS >> 8) & 0xFF)
#define LINK_FREQ_CAP(OFFS) ((OFFS) & 0xFF)

#define PCI_HT_HOST_OFFS LINK_OFFS(		\
		PCI_HT_CAP_HOST_CTRL,           \
		PCI_HT_CAP_HOST_WIDTH,		\
		PCI_HT_CAP_HOST_FREQ,		\
		PCI_HT_CAP_HOST_FREQ_CAP)

#define PCI_HT_SLAVE0_OFFS LINK_OFFS(		\
		PCI_HT_CAP_SLAVE_CTRL0,         \
		PCI_HT_CAP_SLAVE_WIDTH0,	\
		PCI_HT_CAP_SLAVE_FREQ0,		\
		PCI_HT_CAP_SLAVE_FREQ_CAP0)

#define PCI_HT_SLAVE1_OFFS LINK_OFFS(		\
		PCI_HT_CAP_SLAVE_CTRL1,         \
		PCI_HT_CAP_SLAVE_WIDTH1,	\
		PCI_HT_CAP_SLAVE_FREQ1,		\
		PCI_HT_CAP_SLAVE_FREQ_CAP1)

static int ht_optimize_link(
	u32 bdf1, u8 pos1, unsigned offs1,
	u32 bdf2, u8 pos2, unsigned offs2)
{
	static const u8 link_width_to_pow2[]= { 3, 4, 0, 5, 1, 2, 0, 0 };
	static const u8 pow2_to_link_width[] = { 0x7, 4, 5, 0, 1, 3 };
	u16 freq_cap1, freq_cap2;
	u8 width_cap1, width_cap2, width, old_width, ln_width1, ln_width2;
	u8 freq, old_freq;
	int needs_reset;
	/* Set link width and frequency */

	/* Initially assume everything is already optimized and I don't need a reset */
	needs_reset = 0;

	/* Get the frequency capabilities */
	freq_cap1 = ht_read_freq_cap(bdf1, pos1 + LINK_FREQ_CAP(offs1));
	freq_cap2 = ht_read_freq_cap(bdf2, pos2 + LINK_FREQ_CAP(offs2));

	/* Calculate the highest possible frequency */
	freq = log2(freq_cap1 & freq_cap2);

	/* See if I am changing the link freqency */
	old_freq = pci_conf1_read_config8(bdf1, pos1 + LINK_FREQ(offs1));
	old_freq &= 0x0f;
	needs_reset |= old_freq != freq;
	old_freq = pci_conf1_read_config8(bdf2, pos2 + LINK_FREQ(offs2));
	old_freq &= 0x0f;
	needs_reset |= old_freq != freq;

	/* Set the Calulcated link frequency */
	pci_conf1_write_config8(bdf1, pos1 + LINK_FREQ(offs1), freq);
	pci_conf1_write_config8(bdf2, pos2 + LINK_FREQ(offs2), freq);

	/* Get the width capabilities */
	width_cap1 = ht_read_width_cap(bdf1, pos1 + LINK_WIDTH(offs1));
	width_cap2 = ht_read_width_cap(bdf2, pos2 + LINK_WIDTH(offs2));

	/* Calculate dev1's input width */
	ln_width1 = link_width_to_pow2[width_cap1 & 7];
	ln_width2 = link_width_to_pow2[(width_cap2 >> 4) & 7];
	if (ln_width1 > ln_width2) {
		ln_width1 = ln_width2;
	}
	width = pow2_to_link_width[ln_width1];
	/* Calculate dev1's output width */
	ln_width1 = link_width_to_pow2[(width_cap1 >> 4) & 7];
	ln_width2 = link_width_to_pow2[width_cap2 & 7];
	if (ln_width1 > ln_width2) {
		ln_width1 = ln_width2;
	}
	width |= pow2_to_link_width[ln_width1] << 4;

	/* See if I am changing dev1's width */
	old_width = pci_conf1_read_config8(bdf1, pos1 + LINK_WIDTH(offs1) + 1);
	old_width &= 0x77;
	needs_reset |= old_width != width;

	/* Set dev1's widths */
	pci_conf1_write_config8(bdf1, pos1 + LINK_WIDTH(offs1) + 1, width);

	/* Calculate dev2's width */
	width = ((width & 0x70) >> 4) | ((width & 0x7) << 4);

	/* See if I am changing dev2's width */
	old_width = pci_conf1_read_config8(bdf2, pos2 + LINK_WIDTH(offs2) + 1);
	old_width &= 0x77;
	needs_reset |= old_width != width;

	/* Set dev2's widths */
	pci_conf1_write_config8(bdf2, pos2 + LINK_WIDTH(offs2) + 1, width);

	return needs_reset;
}

void ht_setup_chainx(u32 bdf, u8 upos, u8 bus, 
		unsigned offset_unitid, struct sys_info *sysinfo);

int scan_pci_bus( unsigned bus , struct sys_info *sysinfo) 
{
        /*      
                here we already can access PCI_DEV(bus, 0, 0) to PCI_DEV(bus, 0x1f, 0x7)
                So We can scan these devices to find out if they are bridge 
                If it is pci bridge, We need to set busn in bridge, and go on
                For ht bridge, We need to set the busn in bridge and ht_setup_chainx, and the scan_pci_bus
        */    
	unsigned int devfn;
	unsigned new_bus;
	unsigned max_bus;

	new_bus = (bus & 0xff); // mask out the reset_needed

	if(new_bus<0x40) {
		max_bus = 0x3f;
	} else if (new_bus<0x80) {
		max_bus = 0x7f;
	} else if (new_bus<0xc0) {
		max_bus = 0xbf;
	} else {
		max_bus = 0xff;
	}

	new_bus = bus;

	for (devfn = 0; devfn <= 0xff; devfn++) { 
	        u8 hdr_type;
	        u16 class;
		u32 buses;
		u32 bdf;
		u16 cr;
		bdf = PCI_BDF((bus & 0xff), ((devfn>>3) & 0x1f), (devfn & 0x7));
                hdr_type = pci_conf1_read_config8(bdf, PCI_HEADER_TYPE);
                class = pci_conf1_read_config16(bdf, PCI_CLASS_DEVICE);

		switch(hdr_type & 0x7f) {  /* header type */
		        case PCI_HEADER_TYPE_BRIDGE:
		                if (class  != PCI_CLASS_BRIDGE_PCI) goto bad;
				/* set the bus range dev */

			        /* Clear all status bits and turn off memory, I/O and master enables. */
			        cr = pci_conf1_read_config16(bdf, PCI_COMMAND);
			        pci_conf1_write_config16(bdf, PCI_COMMAND, 0x0000);
			        pci_conf1_write_config16(bdf, PCI_STATUS, 0xffff);

			        buses = pci_conf1_read_config32(bdf, PCI_PRIMARY_BUS);

			        buses &= 0xff000000;
				new_bus++;
			        buses |= (((unsigned int) (bus & 0xff) << 0) |
			                ((unsigned int) (new_bus & 0xff) << 8) |
			                ((unsigned int) max_bus << 16));
			        pci_conf1_write_config32(bdf, PCI_PRIMARY_BUS, buses);
				
				/* here we need to figure out if dev is a ht bridge
					if it is ht bridge, we need to call ht_setup_chainx at first
				   Not verified --- yhlu
				*/
				u8 upos;
		                upos = ht_lookup_host_capability(bdf); // one func one ht sub
		                if (upos) { // sub ht chain
					u8 busn;
					busn = (new_bus & 0xff);
			                /* Make certain the HT bus is not enumerated */
			                ht_collapse_previous_enumeration(busn, 0);
					/* scan the ht chain */
			                ht_setup_chainx(bdf,upos,busn, 0, sysinfo); // don't need offset unitid
		                }
							
				new_bus = scan_pci_bus(new_bus, sysinfo);
				/* set real max bus num in that */

			        buses = (buses & 0xff00ffff) |
			                ((unsigned int) (new_bus & 0xff) << 16);
			        pci_conf1_write_config32(bdf, PCI_PRIMARY_BUS, buses);

				pci_conf1_write_config16(bdf, PCI_COMMAND, cr);

                		break;  
        		default:
		        bad:
				;
        	}

                /* if this is not a multi function device, 
                 * or the device is not present don't waste
                 * time probing another function. 
                 * Skip to next device. 
                 */
                if ( ((devfn & 0x07) == 0x00) && ((hdr_type & 0x80) != 0x80))
                {
                        devfn += 0x07;
                }
        }
	
	return new_bus; 
}

void ht_setup_chainx(u32 bdf, u8 upos, u8 bus, 
	unsigned offset_unitid, struct sys_info *sysinfo)
{
	//even HT_CHAIN_UNITID_BASE == 0, we still can go through this function, because of end_of_chain check, also We need it to optimize link

	u8 next_unitid, last_unitid;
	unsigned uoffs;

#if HT_CHAIN_END_UNITID_BASE != 0x20
        /* let's record the device of last ht device, 
	 * So we can set the Unitid to HT_CHAIN_END_UNITID_BASE
	 */
        unsigned real_last_unitid = 0;
        u8 real_last_pos = 0;
	int ht_dev_num = 0;
	u8 end_used = 0;
#endif

	uoffs = PCI_HT_HOST_OFFS;
	next_unitid = (offset_unitid) ? HT_CHAIN_UNITID_BASE:1;

	do {
		u32 id;
		u8 pos;
		u16 flags, ctrl;
		u8 count;
		unsigned offs;
	
		/* Wait until the link initialization is complete */
		do {
			ctrl = pci_conf1_read_config16(bdf, upos + LINK_CTRL(uoffs));
			/* Is this the end of the hypertransport chain? */
			if (ctrl & (1 << 6)) {
				goto end_of_chain;	
			}

			if (ctrl & ((1 << 4) | (1 << 8))) {
                               /*
				* Either the link has failed, or we have
                                * a CRC error.
                                * Sometimes this can happen due to link
                                * retrain, so lets knock it down and see
                                * if its transient
                                */
				ctrl |= ((1 << 4) | (1 <<8)); // Link fail + Crc
                                pci_conf1_write_config16(bdf, upos + LINK_CTRL(uoffs), ctrl);
                                ctrl = pci_conf1_read_config16(bdf, upos + LINK_CTRL(uoffs));
                                if (ctrl & ((1 << 4) | (1 << 8))) {
                                	printk(BIOS_ERR, "Detected error on Hypertransport Link\n");
					break;
                                }
			}
		} while((ctrl & (1 << 5)) == 0);
	
		u32 abdf = PCI_BDF(bus, 0, 0);
		last_unitid = next_unitid;

		id = pci_conf1_read_config32(abdf, PCI_VENDOR_ID);

		/* If the chain is enumerated quit */
		if (    (id == 0xffffffff) || (id == 0x00000000) ||
			(id == 0x0000ffff) || (id == 0xffff0000))
		{
			break;
		}

		pos = ht_lookup_slave_capability(abdf);
		if (!pos) {
			printk(BIOS_ERR, "udev=%08x upos = %08x uoffs = %08x",
				bdf, upos, uoffs);
			printk(BIOS_ERR, ": HT link capability not found\n");
 
		}


#if HT_CHAIN_END_UNITID_BASE != 0x20
		if(offset_unitid) {
			if(next_unitid>= (bus ? 0x20:0x18) ) {
				if(!end_used) {
			                next_unitid = HT_CHAIN_END_UNITID_BASE;
					end_used = 1;
				} else {
					goto out;
				}
				
			} 
        	        real_last_pos = pos;
			real_last_unitid = next_unitid;
			ht_dev_num++;
		} 
#endif
		/* Update the Unitid of the current device */
		flags = pci_conf1_read_config16(abdf, pos + PCI_CAP_FLAGS);
		flags &= ~0x1f; /* mask out the base Unit ID */
		flags |= next_unitid & 0x1f;
		pci_conf1_write_config16(abdf, pos + PCI_CAP_FLAGS, flags);

                /* Compute the number of unitids consumed */
                count = (flags >> 5) & 0x1f;

		/* Note the change in device number */
		abdf = PCI_BDF(bus, next_unitid, 0);

                next_unitid += count;

		/* Find which side of the ht link we are on,
		 * by reading which direction our last write to PCI_CAP_FLAGS
		 * came from.
		 */
		flags = pci_conf1_read_config16(abdf, pos + PCI_CAP_FLAGS);
                offs = ((flags>>10) & 1) ? PCI_HT_SLAVE1_OFFS : PCI_HT_SLAVE0_OFFS;
               
                /* store the link pair here and we will Setup the Hypertransport link later, after we get final FID/VID */
		{
			struct link_pair_st *link_pair = &sysinfo->link_pair[sysinfo->link_pair_num];
			link_pair->udev = bdf;
			link_pair->upos = upos;
			link_pair->uoffs = uoffs;
			link_pair->dev = abdf;
			link_pair->pos = pos;
			link_pair->offs = offs;
			sysinfo->link_pair_num++;
		}

		/* Remeber the location of the last device */
		bdf = abdf;
		upos = pos;
		uoffs = ( offs != PCI_HT_SLAVE0_OFFS ) ? PCI_HT_SLAVE0_OFFS : PCI_HT_SLAVE1_OFFS;

	} while (last_unitid != next_unitid );

#if HT_CHAIN_END_UNITID_BASE != 0x20
out:
#endif
end_of_chain: ;
	
#if HT_CHAIN_END_UNITID_BASE != 0x20
        if(offset_unitid && (ht_dev_num>1) && (real_last_unitid != HT_CHAIN_END_UNITID_BASE) && !end_used ) {
                u16 flags;
		int i;
                flags = pci_conf1_read_config16(PCI_BDF(bus,real_last_unitid,0), real_last_pos + PCI_CAP_FLAGS);
                flags &= ~0x1f;
                flags |= HT_CHAIN_END_UNITID_BASE & 0x1f;
                pci_conf1_write_config16(PCI_BDF(bus, real_last_unitid, 0), real_last_pos + PCI_CAP_FLAGS, flags);

		// Here need to change the dev in the array
		for(i=0;i<sysinfo->link_pair_num;i++)
                {
                        struct link_pair_st *link_pair = &sysinfo->link_pair[i];
                        if(link_pair->udev == PCI_BDF(bus, real_last_unitid, 0)) {
				link_pair->udev = PCI_BDF(bus, HT_CHAIN_END_UNITID_BASE, 0);
				continue;
			}
                        if(link_pair->dev == PCI_BDF(bus, real_last_unitid, 0)) {
                                link_pair->dev = PCI_BDF(bus, HT_CHAIN_END_UNITID_BASE, 0);
                        }
                }

        }
#endif

}

void ht_setup_chain(u32 bdf, unsigned upos, struct sys_info *sysinfo)
{
	unsigned offset_unitid = 0;
#if ((HT_CHAIN_UNITID_BASE != 1) || (HT_CHAIN_END_UNITID_BASE != 0x20))
        offset_unitid = 1;
#endif

        /* Assumption the HT chain that is bus 0 has the HT I/O Hub on it.
         * On most boards this just happens.  If a cpu has multiple
         * non Coherent links the appropriate bus registers for the
         * links needs to be programed to point at bus 0.
         */

        /* Make certain the HT bus is not enumerated */
        ht_collapse_previous_enumeration(0, 0);

#if ((HT_CHAIN_UNITID_BASE != 1) || (HT_CHAIN_END_UNITID_BASE != 0x20))
        offset_unitid = 1;
#endif

	ht_setup_chainx(bdf, upos, 0, offset_unitid, sysinfo);
}

int optimize_link_read_pointer(u8 node, u8 linkn, u8 linkt, u8 val)
{
	u32 dword, dword_old;
	u8 link_type;
	
	/* This works on an Athlon64 because unimplemented links return 0 */
	dword = pci_conf1_read_config32(PCI_BDF(0,0x18+node,0), 0x98 + (linkn * 0x20));
	link_type = dword & 0xff;
	
	
	if ( (link_type & 7) == linkt ) { /* Coherent Link only linkt = 3, ncoherent = 7*/
		dword_old = dword = pci_conf1_read_config32(PCI_BDF(0,0x18+node,3), 0xdc);
		dword &= ~( 0xff<<(linkn *8) );
		dword |= val << (linkn *8);
	
		if (dword != dword_old) {
			pci_conf1_write_config32(PCI_BDF(0,0x18+node,3), 0xdc, dword);
			return 1;
		}
	}
	
	return 0;
}

static int optimize_link_read_pointers_chain(u8 ht_c_num)
{
	int reset_needed; 
	u8 i;

	reset_needed = 0;

	for (i = 0; i < ht_c_num; i++) {
		u32 reg;
		u8 nodeid, linkn;
		u8 busn;
		u8 val;
		unsigned devn = 1;

	#if ((HT_CHAIN_UNITID_BASE != 1) || (HT_CHAIN_END_UNITID_BASE != 0x20))
                #if SB_HT_CHAIN_UNITID_OFFSET_ONLY == 1
                if(i==0) // to check if it is sb ht chain
                #endif
                        devn = HT_CHAIN_UNITID_BASE;
        #endif

		reg = pci_conf1_read_config32(PCI_BDF(0,0x18,1), 0xe0 + i * 4);
		
		nodeid = ((reg & 0xf0)>>4); // nodeid
		linkn = ((reg & 0xf00)>>8); // link n
		busn = (reg & 0xff0000)>>16; //busn

		reg = pci_conf1_read_config32( PCI_BDF(busn, devn, 0), PCI_VENDOR_ID); // ? the chain dev maybe offseted
		if ( (reg & 0xffff) == PCI_VENDOR_ID_AMD) {
			val = 0x25;
		} else if ( (reg & 0xffff) == PCI_VENDOR_ID_NVIDIA ) {
			val = 0x25;//???
		} else {
			continue;
		}

		reset_needed |= optimize_link_read_pointer(nodeid, linkn, 0x07, val);

	}

	return reset_needed;
}

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


static void ht_setup_chains(u8 ht_c_num, struct sys_info *sysinfo)
{
	/* Assumption the HT chain that is bus 0 has the HT I/O Hub on it. 
	 * On most boards this just happens.  If a cpu has multiple
	 * non Coherent links the appropriate bus registers for the
	 * links needs to be programed to point at bus 0.
	 */
        u8 upos;
        u32 ubdf;
	u8 i;

	sysinfo->link_pair_num = 0;

	// first one is SB Chain
	for (i = 0; i < ht_c_num; i++) {
		u32 reg;
		u8 devpos;
		unsigned regpos;
		u32 dword;
		u8 busn;
		unsigned offset_unitid = 0;
		
		reg = pci_conf1_read_config32(PCI_BDF(0,0x18,1), 0xe0 + i * 4);

		//We need setup 0x94, 0xb4, and 0xd4 according to the reg
		devpos = ((reg & 0xf0)>>4)+0x18; // nodeid; it will decide 0x18 or 0x19
		regpos = ((reg & 0xf00)>>8) * 0x20 + 0x94; // link n; it will decide 0x94 or 0xb4, 0x0xd4;
		busn = (reg & 0xff0000)>>16;
		
		dword = pci_conf1_read_config32( PCI_BDF(0, devpos, 0), regpos) ;
		dword &= ~(0xffff<<8);
		dword |= (reg & 0xffff0000)>>8;
		pci_conf1_write_config32( PCI_BDF(0, devpos,0), regpos , dword);
	

	#if ((HT_CHAIN_UNITID_BASE != 1) || (HT_CHAIN_END_UNITID_BASE != 0x20))
                #if SB_HT_CHAIN_UNITID_OFFSET_ONLY == 1
                if(i==0) // to check if it is sb ht chain
                #endif
                        offset_unitid = 1;
        #endif
	
	        /* Make certain the HT bus is not enumerated */
        	ht_collapse_previous_enumeration(busn, offset_unitid);

		upos = ((reg & 0xf00)>>8) * 0x20 + 0x80;
		ubdf =  PCI_BDF(0, devpos, 0);

		ht_setup_chainx(ubdf,upos,busn, offset_unitid, sysinfo); // all not

		#if (CONFIG_K8_SCAN_PCI_BUS == 1)
		bus = busn; // we need 32 bit 
    	    	scan_pci_bus(bus, sysinfo);
		#endif
	}

}

unsigned int get_nodes(void);

void ht_setup_chains_x(struct sys_info *sysinfo)
{
	unsigned int get_sbdn(unsigned int bus); 
        u8 nodeid;
        u32 reg; 
	u32 tempreg;
        u8 next_busn;
        u8 ht_c_num;
	u8 nodes;
#if CONFIG_K8_ALLOCATE_IO_RANGE == 1	
	unsigned next_io_base;
#endif

	nodes = get_nodes();     
 
        /* read PCI_DEV(0,0x18,0) 0x64 bit [8:9] to find out SbLink m */
        reg = pci_conf1_read_config32(PCI_BDF(0, 0x18, 0), 0x64);
        /* update PCI_DEV(0, 0x18, 1) 0xe0 to 0x05000m03, and next_busn=0x3f+1 */
	print_linkn_in("SBLink=", ((reg>>8) & 3) );
	sysinfo->sblk = (reg>>8) & 3;
	sysinfo->sbbusn = 0;
	sysinfo->nodes = nodes;
        tempreg = 3 | ( 0<<4) | (((reg>>8) & 3)<<8) | (0<<16)| (0x3f<<24);
        pci_conf1_write_config32(PCI_BDF(0, 0x18, 1), 0xe0, tempreg);

	next_busn=0x3f+1; /* 0 will be used ht chain with SB we need to keep SB in bus0 in auto stage*/

#if CONFIG_K8_ALLOCATE_IO_RANGE == 1
	/* io range allocation */
	tempreg = 0 | (((reg>>8) & 0x3) << 4 )|  (0x3<<12); //limit
	pci_conf1_write_config32(PCI_BDF(0, 0x18, 1), 0xC4, tempreg);
	tempreg = 3 | ( 3<<4) | (0<<12);	//base
	pci_conf1_write_config32(PCI_BDF(0, 0x18, 1), 0xC0, tempreg);
	next_io_base = 0x3+0x1;
#endif

	/* clean others */
        for(ht_c_num=1;ht_c_num<4; ht_c_num++) {
                pci_conf1_write_config32(PCI_BDF(0, 0x18, 1), 0xe0 + ht_c_num * 4, 0);

#if CONFIG_K8_ALLOCATE_IO_RANGE == 1
		/* io range allocation */
		pci_conf1_write_config32(PCI_BDF(0, 0x18, 1), 0xc4 + ht_c_num * 8, 0);
		pci_conf1_write_config32(PCI_BDF(0, 0x18, 1), 0xc0 + ht_c_num * 8, 0);
#endif
        }
 
        for(nodeid=0; nodeid<nodes; nodeid++) {
                u32 bdf; 
                u8 linkn;
                bdf = PCI_BDF(0, 0x18+nodeid,0);
                for(linkn = 0; linkn<3; linkn++) {
                        unsigned regpos;
                        regpos = 0x98 + 0x20 * linkn;
                        reg = pci_conf1_read_config32(bdf, regpos);
                        if ((reg & 0x17) != 7) continue; /* it is not non conherent or not connected*/
			print_linkn_in("NC node|link=", ((nodeid & 0xf)<<4)|(linkn & 0xf));
                        tempreg = 3 | (nodeid <<4) | (linkn<<8);
                        /*compare (temp & 0xffff), with (PCI(0, 0x18, 1) 0xe0 to 0xec & 0xfffff) */
                        for(ht_c_num=0;ht_c_num<4; ht_c_num++) {
                                reg = pci_conf1_read_config32(PCI_BDF(0, 0x18, 1), 0xe0 + ht_c_num * 4);
                                if(((reg & 0xffff) == (tempreg & 0xffff)) || ((reg & 0xffff) == 0x0000)) {  /*we got it*/
                                        break;
                                }
                        }
                        if(ht_c_num == 4) break; /*used up only 4 non conherent allowed*/
                        /*update to 0xe0...*/
			if((reg & 0xf) == 3) continue; /*SbLink so don't touch it */
			print_linkn_in("\tbusn=", next_busn);
                        tempreg |= (next_busn<<16)|((next_busn+0x3f)<<24);
                        pci_conf1_write_config32(PCI_BDF(0, 0x18, 1), 0xe0 + ht_c_num * 4, tempreg);
			next_busn+=0x3f+1;

#if CONFIG_K8_ALLOCATE_IO_RANGE == 1			
			/* io range allocation */
		        tempreg = nodeid | (linkn<<4) |  ((next_io_base+0x3)<<12); //limit
		        pci_conf1_write_config32(PCI_BDF(0, 0x18, 1), 0xC4 + ht_c_num * 8, tempreg);
		        tempreg = 3 /*| ( 3<<4)*/ | (next_io_base<<12);        //base :ISA and VGA ?
		        pci_conf1_write_config32(PCI_BDF(0, 0x18, 1), 0xC0 + ht_c_num * 8, tempreg);
		        next_io_base += 0x3+0x1;
#endif

                }
        }
        /*update 0xe0, 0xe4, 0xe8, 0xec from PCI_DEV(0, 0x18,1) to PCI_DEV(0, 0x19,1) to PCI_DEV(0, 0x1f,1);*/

        for(nodeid = 1; nodeid<nodes; nodeid++) {
                int i;
                u32 dev;
                dev = PCI_BDF(0, 0x18+nodeid,1);
                for(i = 0; i< 4; i++) {
                        unsigned regpos;
                        regpos = 0xe0 + i * 4;
                        reg = pci_conf1_read_config32(PCI_BDF(0, 0x18, 1), regpos);
                        pci_conf1_write_config32(dev, regpos, reg);
                }

#if CONFIG_K8_ALLOCATE_IO_RANGE == 1
		/* io range allocation */
                for(i = 0; i< 4; i++) {
                        unsigned regpos;
                        regpos = 0xc4 + i * 8;
                        reg = pci_conf1_read_config32(PCI_BDF(0, 0x18, 1), regpos);
                        pci_conf1_write_config32(dev, regpos, reg);
                }
                for(i = 0; i< 4; i++) {
                        unsigned regpos;
                        regpos = 0xc0 + i * 8;
                        reg = pci_conf1_read_config32(PCI_BDF(0, 0x18, 1), regpos);
                        pci_conf1_write_config32(dev, regpos, reg);
                }
#endif
        }
	
	/* recount ht_c_num*/
	u8 i=0;
        for(ht_c_num=0;ht_c_num<4; ht_c_num++) {
		reg = pci_conf1_read_config32(PCI_BDF(0, 0x18, 1), 0xe0 + ht_c_num * 4);
                if(((reg & 0xf) != 0x0)) {
			i++;
		}
        }

	sysinfo->ht_c_num = i;
        ht_setup_chains(i, sysinfo);
	sysinfo->sbdn = get_sbdn(sysinfo->sbbusn);

}

int optimize_link_incoherent_ht(struct sys_info *sysinfo)
{
	// We need to use recorded link pair info to optimize the link
	int i;
	int reset_needed = 0;
	
	unsigned link_pair_num = sysinfo->link_pair_num;

	for(i=0; i< link_pair_num; i++) {	
		struct link_pair_st *link_pair= &sysinfo->link_pair[i];
		reset_needed |= ht_optimize_link(link_pair->udev, link_pair->upos, link_pair->uoffs, link_pair->dev, link_pair->pos, link_pair->offs);
	}

	reset_needed |= optimize_link_read_pointers_chain(sysinfo->ht_c_num);

	return reset_needed;

}



