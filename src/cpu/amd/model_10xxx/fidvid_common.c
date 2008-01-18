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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#include <cpu/x86/tsc.h>


static u32 get_vstime(u32 nodeid, u32 slam)
{
	u32 val;
	u32 v;
	device_t dev;

#if defined(__ROMCC__)
	dev = NODE_PCI(nodeid, 3);
#else
	dev = get_node_pci(nodeid, 3);
#endif

	val = pci_read_config32(dev, 0xd8);

	val >>= slam?0:4;
	val &= 7;

	switch (val) {
	case 4: v = 60; break;
	case 5: v = 100; break;
	case 6: v = 200; break;
	case 7: v = 500; break;
	default:
		v = (val+1)*10; // in us
	}

	return v;
}

static void udelay_tsc(u32 us)
{
	/* Use TSC to delay because it is fixed, ie. it will not changed with p-states.
	 * Also, We use the APIC TIMER register is to hold flags for AP init.
	 */
	u32 dword;
	tsc_t tsc, tsc1, tscd;
	u32 d =  0x00000200; //800Mhz or 200Mhz or 1.6G or get the NBFID at first
	u32 dn = 0x1000000/2; // howmany us need to use hi

	tscd.hi = us/dn;
	tscd.lo = (us - tscd.hi * dn) * d;

	tsc1 = rdtsc();
	dword = tsc1.lo + tscd.lo;
	if((dword<tsc1.lo) || (dword<tscd.lo)) {
		tsc1.hi++;
	}
	tsc1.lo = dword;
	tsc1.hi+= tscd.hi;

	do {
		tsc = rdtsc();
	} while ((tsc.hi>tsc1.hi) || ((tsc.hi==tsc1.hi) && (tsc.lo>tsc1.lo)));

}

#ifdef __ROMCC__
void udelay(u32 usecs)
{
	udelay_tsc(usecs);
}
#endif

static u32 set_vid(u32 newvid, u32 bit_offset, u32 nodeid, u32 coreid)
{
	u32 val;
	msr_t msr;
	u32 curvid;
	u32 slam;
	u32 delay;
	u32 count = 3;
	device_t dev;

	msr = rdmsr(0xc0010071);//status
	curvid = (msr.lo >> bit_offset) & 0x7f; // seven bits

	if(newvid == curvid) return curvid;

#if defined(__ROMCC__)
	dev = NODE_PCI(nodeid, 3);
#else
	dev = get_node_pci(nodeid, 3);
#endif

	val = pci_read_config32(dev, 0xa0);

	slam = (val >> 29) & 1;
	delay = get_vstime(nodeid, slam);

	if(!slam) {
		if(curvid>newvid) {
			count = (curvid - newvid) * 2;
		} else {
			count = (newvid - curvid) * 2;
		}
	}

	while(count-->0) {
		if(slam) {
			curvid = newvid;
		}
		else { //ramp
			if(curvid>newvid) {
				curvid--;
			} else {
				curvid++;
			}
		}

		msr = rdmsr(0xc0010070); //control
		msr.lo &= ~(0x7f<<bit_offset);
		msr.lo |= (curvid<<bit_offset);
		wrmsr(0xc0010070, msr); // how about all copys, APIC or PCI conf space?

		udelay_tsc(delay);

		msr = rdmsr(0xc0010071);//status
		curvid = (msr.lo >> bit_offset) & 0x7f; // seven bits

		if(curvid == newvid) break;

	}

	return curvid;
}


static u32 set_nb_vid(u32 newvid, u32 nodeid, u32 coreid)
{
	return set_vid(newvid, 25, nodeid, coreid);
}


static u32 set_core_vid(u32 newvid, u32 nodeid, u32 coreid)
{
	return set_vid(newvid, 9, nodeid, coreid);
}


static unsigned set_cof(u32 val, u32 mask, u32 nodeid, u32 coreid)
{
	msr_t msr;
	int count = 3;

	val &= mask;

	// FIXME: What is count for? Why 3 times? What about node and core id?
	while(count-- > 0) {

		msr = rdmsr(0xc0010071);
		msr.lo &= mask;
		if(msr.lo == val) break;

		msr = rdmsr(0xc0010070);
		msr.lo &= ~(mask);
		msr.lo |= val;
		wrmsr(0xc0010070, msr);
	}

	return msr.lo;
}

static u32 set_core_cof(u32 fid, u32 did, u32 nodeid, u32 coreid)
{
	u32 val;
	u32 mask;

	mask = (7<<6) | 0x3f;
	val = ((did & 7)<<6) | (fid & 0x3f);

	return set_cof(val, mask, nodeid, coreid);

}


static u32 set_nb_cof(u32 did, u32 nodeid, u32 coreid) // fid need warmreset
{
	u32 val;
	u32 mask;

	mask = 1<<22;
	val = (did & 1)<<22;

	return set_cof(val, mask, nodeid, coreid);

}


/* set vid and cof for core and nb after warm reset is not started by BIOS */
static void set_core_nb_max_pstate_after_other_warm_reset(u32 nodeid, u32 coreid) // P0
{
	msr_t msr;
	u32 val;
	u32 vid;
	u32 mask;
	u32 did;
	device_t dev;

	msr = rdmsr(0xc0010064);

#if defined(__ROMCC__)
	dev = NODE_PCI(nodeid, 3);
#else
	dev = get_node_pci(nodeid, 3);
#endif

	val = pci_read_config32(dev, 0xa0);
	if((val>>8) & 1) { // PVI
		vid = (msr.lo >> 25) & 0x7f;
	} else { //SVI
		vid = (msr.lo >> 9) & 0x7f;
	}
	set_core_vid(vid, nodeid, coreid);

	mask = (0x7<<6) | 0x3f;
	val = msr.lo & mask;
	set_cof(val, mask, nodeid, coreid);

	//set nb cof and vid
	did = (msr.lo >> 22) & 1;
	vid = (msr.lo >> 25) & 0x7f;
	if(did) {
		 set_nb_cof(did, nodeid, coreid);
		set_nb_vid(vid, nodeid, coreid);
	} else {
		set_nb_vid(vid, nodeid, coreid);
		 set_nb_cof(did, nodeid, coreid);
	}

	//set the p state
	msr.hi = 0;
	msr.lo = 0;
	wrmsr(0xc0010062, msr);

}


/* set vid and cof for core and nb after warm reset is not started by BIOS */
static void  set_core_nb_min_pstate_after_other_warm_reset(u32 nodeid, u32 coreid) // Px
{
	msr_t msr;
	u32 val;
	u32 vid;
	u32 mask;
	u32 did;
	u32 pstate;
	device_t dev;

#if defined(__ROMCC__)
	dev = NODE_PCI(nodeid, 3);
#else
	dev = get_node_pci(nodeid, 3);
#endif


	val = pci_read_config32(dev, 0xdc); //PstateMaxVal

	pstate = (val >> 8) & 0x7;

	msr = rdmsr(0xc0010064 + pstate);

	mask = (7<<6) | 0x3f;
	val = msr.lo & mask;
	set_cof(val, mask, nodeid, coreid);

	val = pci_read_config32(dev, 0xa0);
	if((val>>8) & 1) { // PVI
		 vid = (msr.lo>>25) & 0x7f;
	} else { //SVI
		 vid = (msr.lo>>9) & 0x7f;
	}
	set_core_vid(vid, nodeid, coreid);

	//set nb cof and vid
	did = (msr.lo >> 22) & 1;
	vid = (msr.lo >> 25) & 0x7f;
	if(did) {
		set_nb_cof(did, nodeid, coreid);
		set_nb_vid(vid, nodeid, coreid);
	} else {
		set_nb_vid(vid, nodeid, coreid);
		set_nb_cof(did, nodeid, coreid);
	}

	//set the p state
	msr.hi = 0;
	msr.lo = pstate;
	wrmsr(0xc0010062, msr);
}
