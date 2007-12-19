/*
 * This file is part of the LinuxBIOS project.
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

#if FAM10_SET_FIDVID == 1

#define FAM10_SET_FIDVID_DEBUG 1

// if we are tight of CAR stack, disable it
#define FAM10_SET_FIDVID_STORE_AP_APICID_AT_FIRST 1

static inline void print_debug_fv(const char *str, u32 val)
{
#if FAM10_SET_FIDVID_DEBUG == 1
		printk_debug("%s%x\n", str, val);
#endif
}

static inline void print_debug_fv_8(const char *str, u8 val)
{
#if FAM10_SET_FIDVID_DEBUG == 1
		printk_debug("%s%02x\n", str, val);
#endif
}

static inline void print_debug_fv_64(const char *str, u32 val, u32 val2)
{
#if FAM10_SET_FIDVID_DEBUG == 1
		printk_debug("%s%x%x\n", str, val, val2);
#endif
}


static void enable_fid_change(u8 fid)
{
	u32 dword;
	u32 nodes;
	device_t dev;
	int i;

	nodes = ((pci_read_config32(PCI_DEV(CBB, CDB, 0), 0x60) >> 4) & 7) + 1;

	for(i = 0; i < nodes; i++) {
		dev = NODE_PCI(i,3);
		dword = pci_read_config32(dev, 0xd4);
		dword &= ~0x1F;
		dword |= (u32) fid & 0x1F;
		dword |= 1 << 5;	// enable
		pci_write_config32(dev, 0xd4, dword);
		printk_debug("FID Change Node:%02x, F3xD4: %08x \n", i, dword);
	}
}

static void prep_fid_change(void)
{
	u32 dword;
	u32 nodes;
	device_t dev;
	int i;

	/* This needs to be run before any Pstate changes are requested */

	nodes = ((pci_read_config32(PCI_DEV(CBB, CDB, 0), 0x60) >> 4) & 7) + 1;

	for(i = 0; i < nodes; i++) {
		printk_debug("Node:%02x \n", i);
		dev = NODE_PCI(i,3);

		dword = pci_read_config32(dev, 0xa0);
		dword &= ~(1<<29);
		dword |= ((~dword >> 8) & 1) << 29; // SlamVidMode is the inverse to the PviMode
		dword |= PLLLOCK_DFT_L; /* Force per BKDG */
		pci_write_config32(dev, 0xa0, dword);
		printk_debug("  F3xA0: %08x \n", dword);

		dword = pci_read_config32(dev, 0xd8);
		dword &= ~0x77;
		dword |= (1<<4) | 6; // VSRampTime, and VSSlamTime
		dword |= 3 << 24; // ReConDel set to 3 per BKDG
		pci_write_config32(dev, 0xd8, dword);
		printk_debug("  F3xD8: %08x \n", dword);

		dword = pci_read_config32(dev, 0xd4);
		dword &= 0x1F;
		dword |= 0xC331AF00; // per BKDG
		pci_write_config32(dev, 0xd4, dword);
		printk_debug("  F3xD4: %08x \n", dword);

		dword = pci_read_config32(dev, 0xdc);
		dword |= 0x5 << 12; // NbsynPtrAdj set to 0x5 per BKDG (needs reset)
		pci_write_config32(dev, 0xdc, dword);
		printk_debug("  F3xDC: %08x \n", dword);

		// Rev B settings - FIXME: support other revs.
		dword = 0xA0E641E6;
		pci_write_config32(dev, 0x84, dword);
		printk_debug("  F3x84: %08x \n", dword);

		dword = 0xE600A681;
		pci_write_config32(dev, 0x80, dword);
		printk_debug("  F3x80: %08x \n", dword);

	}
}


#include "fidvid_common.c"



static void init_fidvid_ap(u32 bsp_apicid, u32 apicid, u32 nodeid, u32 coreid)
{

	msr_t msr;
	device_t dev;
	u8 vid_max;
	u8 fid_max;
	u8 startup_pstate;
	u8 nb_cof_vid_update;
	u8 pvimode;
	u32 reg1fc;
	u32 dword;
	u32 send;

	printk_debug("FIDVID on AP: %02x\n", apicid);

	/* Only support single plane system at this time. */
	/* Steps 1-6 of BIOS NB COF and VID Configuration
	 * for Single-Plane PVI Systems
	 */
	dev = NODE_PCI(nodeid,3);
	reg1fc = pci_read_config32(dev, 0x1FC);
	nb_cof_vid_update = reg1fc & 1;
	if (nb_cof_vid_update) {
		/* Get fused settings */
		dword = pci_read_config32(dev, 0xa0);
			pvimode = (dword >> 8) & 1;

		vid_max = (reg1fc >> 7) & 0x7F;	// per node
		fid_max = (reg1fc >> 2) & 0x1F;	// per system

		if (pvimode) {
		/* FIXME: support daul plane mode */
			die("PVImode not supported\n");
			/* fidmax = vidmax - (reg1fc >> 17) & 0x1F;
			fidmax = fidmax + (reg1fc >> 14) & 0x03;
			*/
		}

	} else {
		/* Use current values */
		msr = rdmsr(0xc0010071);
		fid_max = ((msr.hi >> (59-32)) & 0x1f);		//max nb fid
		vid_max = ((msr.hi >> (35-32)) & 0x7f);		//max vid
	}

	/* Note this is the single plane setup. Need to add dual plane path */
	msr = rdmsr(0xc0010071);
	startup_pstate = (msr.hi >> (32-32)) & 0x07;


	/* Copy startup pstate to P1 and P0 MSRs. Set the maxvid for this node in P0.
	   Then transition to P1 for corex and P0 for core0. */
	msr = rdmsr(0xC0010064 + startup_pstate);
	wrmsr(0xC0010065, msr);
	wrmsr(0xC0010064, msr);

	msr.lo &= ~0xFE000000;	// clear nbvid
	msr.lo |= vid_max << 25;
	wrmsr(0xC0010064, msr);

	// Transition to P1 for all APs and P0 for core0.
	msr = rdmsr(0xC0010062);
	msr.lo = (msr.lo & ~0x07) | 1;
	wrmsr(0xC0010062, msr);

	// Wait for P1 to set.
	do {
		msr = rdmsr(0xC0010063);
	} while (msr.lo != 1);

	if (coreid == 0) {
		msr.lo = msr.lo & ~0x07;
		wrmsr(0xC0010062, msr);
		// Wait for P0 to set.
		do {
			msr = rdmsr(0xC0010063);
		} while (msr.lo != 0);
	}


	send = (nb_cof_vid_update << 16) | (fid_max << 8);
	send |= (apicid << 24); // ap apicid

	// Send signal to BSP about this AP max fid
	// This also indicates this AP is ready for warm reset (if required).
	lapic_write(LAPIC_MSG_REG, send | 1);
}

static u32 calc_common_fid(u32 fid_packed, u32 fid_packed_new)
{
	u32 fidmax;
	u32 fidmax_new;

	fidmax = (fid_packed >> 8) & 0xFF;

	fidmax_new = (fid_packed_new >> 8) & 0xFF;

	if(fidmax > fidmax_new) {
		fidmax = fidmax_new;
	}

	fid_packed &= 0xFF << 16;
	fid_packed |= (fidmax << 8);
	fid_packed |= fid_packed_new & (0xFF << 16); // set nb_cof_vid_update

	return fid_packed;
}

struct fidvid_st {
	u32 common_fid;
};

static void init_fidvid_bsp_stage1(u32 ap_apicid, void *gp )
{
		u32 readback = 0;
		u32 timeout = 1;

		struct fidvid_st *fvp = gp;
		int loop;

		print_debug_fv("Wait for AP stage 1: ap_apicid = ", ap_apicid);

		loop = 100000;
		while(--loop > 0) {
			if(lapic_remote_read(ap_apicid, LAPIC_MSG_REG, &readback) != 0) continue;
			if((readback & 0x3f) == 1) {
				timeout = 0;
				break; //target ap is in stage 1
			}
		}

		if(timeout) {
			print_initcpu8("fidvid_bsp_stage1: time out while reading from ap ", ap_apicid);
			return;
		}

		print_debug_fv("\treadback = ", readback);

		fvp->common_fid = calc_common_fid(fvp->common_fid, readback);

		print_debug_fv("\tcommon_fid(packed) = ", fvp->common_fid);

}


static void init_fidvid_stage2(u32 apicid, u32 nodeid)
{
	msr_t msr;
	device_t dev;
	u32 reg1fc;
	u8 StartupPstate;
	u8 nbvid;
	int i;

	/* After warm reset finish the fid/vid setup for all cores. */
	dev = NODE_PCI(nodeid,3);
	reg1fc = pci_read_config32(dev, 0x1FC);
	nbvid = (reg1fc >> 7) & 0x7F;

	if (reg1fc & 0x02) { // NbVidUpdateAll ?
		for( i = 0; i < 5; i++) {
			msr = rdmsr(0xC0010064 + i);
			if ((msr.hi >> 31) & 1) { // PstateEn?
				msr.lo &= ~(0x7F << 25);
				msr.lo |= (nbvid & 0x7F) << 25;
			}
		}
	} else {
		for( i = 0; i < 5; i++) {
			msr = rdmsr(0xC0010064 + i);
			if (((msr.hi >> 31) & 1) && (((msr.lo >> 22) & 1) == 0)) { // PstateEn and PDid == 0?
				msr.lo &= ~(0x7F << 25);
				msr.lo |= (nbvid & 0x7F) << 25;
			}
		}
	}

	// For each processor in the system, transition all cores to StartupPstate
	msr = rdmsr(0xC0010071);
	StartupPstate = msr.hi >> (32-32) & 0x03;
	msr = rdmsr(0xC0010062);
	msr.lo = StartupPstate;
	wrmsr(0xC0010062, msr);
}


#if FAM10_SET_FIDVID_STORE_AP_APICID_AT_FIRST == 1
struct ap_apicid_st {
	u32 num;
	// it could use 256 bytes for 64 node quad core system
	u8 apicid[NODE_NUMS * 4];
};

static void store_ap_apicid(unsigned ap_apicid, void *gp)
{
	struct ap_apicid_st *p = gp;

	p->apicid[p->num++] = ap_apicid;

}
#endif


static int init_fidvid_bsp(u32 bsp_apicid, u32 nodes)
{
#if FAM10_SET_FIDVID_STORE_AP_APICID_AT_FIRST == 1
	struct ap_apicid_st ap_apicidx;
	u32 i;
#endif
	struct fidvid_st fv;
	msr_t msr;
	device_t dev;
	u8 vid_max;
	u8 fid_max;
	u8 startup_pstate;
	u8 nb_cof_vid_update;
	u32 reg1fc;
	u32 dword;
	u8 pvimode;

	printk_debug("FIDVID on BSP, APIC_id: %02x\n", bsp_apicid);

	/* FIXME: Only support single plane system at this time. */
	/* Steps 1-6 of BIOS NB COF and VID Configuration
	 * for Single-Plane PVI Systems
	 */
	dev = NODE_PCI(0,3);	// nodeid for the BSP is 0
	reg1fc = pci_read_config32(dev, 0x1FC);
	nb_cof_vid_update = reg1fc & 1;
	if (nb_cof_vid_update) {
		/* Get fused settings */
		dword = pci_read_config32(dev, 0xa0);
			pvimode = (dword >> 8) & 1;

		vid_max = (reg1fc >> 7) & 0x7F;	// per node
		fid_max = (reg1fc >> 2) & 0x1F;	// per system

		if (pvimode) {
		/* FIXME: support daul plane mode */
			die("PVImode not supported\n");
			/* fidmax = vidmax - (reg1fc >> 17) & 0x1F;
			fidmax = fidmax + (reg1fc >> 14) & 0x03;
			*/
		}

	} else {
		/* Use current values */
		msr = rdmsr(0xc0010071);
		fid_max = ((msr.hi >> (59-32)) & 0x1f);		//max nb fid
		vid_max = ((msr.hi >> (35-32)) & 0x7f);		//max vid
	}

	/* Note this is the single plane setup. Need to add dual plane path */
	msr = rdmsr(0xc0010071);
	startup_pstate = (msr.hi >> (32-32)) & 0x07;


	/* Copy startup pstate to P1 and P0 MSRs. Set the maxvid for this node in P0.
	   Then transition to P1 for corex and P0 for core0. */
	msr = rdmsr(0xC0010064 + startup_pstate);
	wrmsr(0xC0010065, msr);
	wrmsr(0xC0010064, msr);

	msr.lo &= ~0xFE000000;	// clear nbvid
	msr.lo |= vid_max << 25;
	wrmsr(0xC0010064, msr);

	// Transition to P1 and then P0 for core0.
	msr = rdmsr(0xC0010062);
	msr.lo = (msr.lo & ~0x07) | 1;
	wrmsr(0xC0010062, msr);

	// Wait for P1 to set.
	do {
		msr = rdmsr(0xC0010063);
	} while (msr.lo != 1);

	msr.lo = msr.lo & ~0x07;
	wrmsr(0xC0010062, msr);
	// Wait for P0 to set.
	do {
		msr = rdmsr(0xC0010063);
	} while (msr.lo != 0);


	fv.common_fid = (nb_cof_vid_update << 16) | (fid_max << 8) ;
	print_debug_fv("BSP fid = ", fv.common_fid);

#if FAM10_SET_FIDVID_STORE_AP_APICID_AT_FIRST == 1 && FAM10_SET_FIDVID_CORE0_ONLY == 0
	/* For all APs (We know the APIC ID of all APs even when the APIC ID
	   is lifted) remote read from AP LAPIC_MSG_REG about max fid.
	   Then calculate the common max fid that can be used for all
	   APs and BSP */
	ap_apicidx.num = 0;

	for_each_ap(bsp_apicid, FAM10_SET_FIDVID_CORE_RANGE, store_ap_apicid, &ap_apicidx);

	for(i = 0; i < ap_apicidx.num; i++) {
		init_fidvid_bsp_stage1(ap_apicidx.apicid[i], &fv);
	}
#else
	for_each_ap(bsp_apicid, FAM10_SET_FIDVID_CORE0_ONLY, init_fidvid_bsp_stage1, &fv);
#endif

	print_debug_fv("common_fid = ", fv.common_fid);

	if (fv.common_fid & ~(0xFF << 16)) {	// check nb_cof_vid_update

		// Enable the common fid and other settings.
		enable_fid_change((fv.common_fid >> 8) & 0x1F);

		// nbfid change need warm reset, so reset at first
		return 1;
	}

	return 0; // No FID/VID changes. Don't reset
}
static void set_p0(void)
{
	msr_t msr;

	// Transition P0 for calling core.
	msr = rdmsr(0xC0010062);
	msr.lo = (msr.lo & ~0x07);
	wrmsr(0xC0010062, msr);

	// Don't bother to wait around for the P state to change.
}
#endif
