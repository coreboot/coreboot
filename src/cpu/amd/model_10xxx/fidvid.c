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

#if CONFIG_SET_FIDVID
#include <northbridge/amd/amdht/AsPsDefs.h>

static inline void print_debug_fv(const char *str, u32 val)
{
#if CONFIG_SET_FIDVID_DEBUG
	printk(BIOS_DEBUG, "%s%x\n", str, val);
#endif
}

static inline void print_debug_fv_8(const char *str, u8 val)
{
#if CONFIG_SET_FIDVID_DEBUG
	printk(BIOS_DEBUG, "%s%02x\n", str, val);
#endif
}

static inline void print_debug_fv_64(const char *str, u32 val, u32 val2)
{
#if CONFIG_SET_FIDVID_DEBUG
	printk(BIOS_DEBUG, "%s%x%x\n", str, val, val2);
#endif
}

struct fidvid_st {
	u32 common_fid;
};

static void enable_fid_change(u8 fid)
{
	u32 dword;
	u32 nodes;
	device_t dev;
	int i;

	nodes = get_nodes();

	for (i = 0; i < nodes; i++) {
		dev = NODE_PCI(i, 3);
		dword = pci_read_config32(dev, 0xd4);
		dword &= ~0x1F;
		dword |= (u32) fid & 0x1F;
		dword |= 1 << 5;	// enable
		pci_write_config32(dev, 0xd4, dword);
		printk(BIOS_DEBUG, "FID Change Node:%02x, F3xD4: %08x \n", i,
		       dword);
	}
}

static void setVSRamp(device_t dev) {
	/* BKDG r31116 2010-04-22  2.4.1.7 step b F3xD8[VSRampTime] 
         * If this field accepts 8 values between 10 and 500 us why 
         * does page 324 say "BIOS should set this field to 001b." 
         * (20 us) ?
         * Shouldn't it depend on the voltage regulators, mainboard
         * or something ? 
         */ 
        u32 dword;
	dword = pci_read_config32(dev, 0xd8);
	dword &= VSRAMP_MASK;
	dword |= VSRAMP_VALUE;
	pci_write_config32(dev, 0xd8, dword);
}

static void recalculateVsSlamTimeSettingOnCorePre(device_t dev)
{
	u8 pviModeFlag;
	u8 highVoltageVid, lowVoltageVid, bValue;
	u16 minimumSlamTime;
	u16 vSlamTimes[7] = { 1000, 2000, 3000, 4000, 6000, 10000, 20000 };	/* Reg settings scaled by 100 */
	u32 dtemp;
	msr_t msr;

	/* This function calculates the VsSlamTime using the range of possible
	 * voltages instead of a hardcoded 200us.
	 * Note:This function is called from setFidVidRegs and setUserPs after
	 * programming a custom Pstate.
	 */

	/* Calculate Slam Time
	 * Vslam = 0.4us/mV * Vp0 - (lowest out of Vpmin or Valt)
	 * In our case, we will scale the values by 100 to avoid
	 * decimals.
	 */

	/* Determine if this is a PVI or SVI system */
	dtemp = pci_read_config32(dev, 0xA0);

	if (dtemp & PVI_MODE)
		pviModeFlag = 1;
	else
		pviModeFlag = 0;

	/* Get P0's voltage */
	msr = rdmsr(0xC0010064);
	highVoltageVid = (u8) ((msr.lo >> PS_CPU_VID_SHFT) & 0x7F);

	/* If SVI, we only care about CPU VID.
	 * If PVI, determine the higher voltage b/t NB and CPU
	 */
	if (pviModeFlag) {
		bValue = (u8) ((msr.lo >> PS_NB_VID_SHFT) & 0x7F);
		if (highVoltageVid > bValue)
			highVoltageVid = bValue;
	}

	/* Get Pmin's index */
	msr = rdmsr(0xC0010061);
	bValue = (u8) ((msr.lo >> PS_CUR_LIM_SHFT) & BIT_MASK_3);

	/* Get Pmin's VID */
	msr = rdmsr(0xC0010064 + bValue);
	lowVoltageVid = (u8) ((msr.lo >> PS_CPU_VID_SHFT) & 0x7F);

	/* If SVI, we only care about CPU VID.
	 * If PVI, determine the higher voltage b/t NB and CPU
	 */
	if (pviModeFlag) {
		bValue = (u8) ((msr.lo >> PS_NB_VID_SHFT) & 0x7F);
		if (lowVoltageVid > bValue)
			lowVoltageVid = bValue;
	}

	/* Get AltVID */
	dtemp = pci_read_config32(dev, 0xDC);
	bValue = (u8) (dtemp & BIT_MASK_7);

	/* Use the VID with the lowest voltage (higher VID) */
	if (lowVoltageVid < bValue)
		lowVoltageVid = bValue;

	/* If Vids are 7Dh - 7Fh, force 7Ch to keep calculations linear */
	if (lowVoltageVid > 0x7C) {
		lowVoltageVid = 0x7C;
		if (highVoltageVid > 0x7C)
			highVoltageVid = 0x7C;
	}

	bValue = (u8) (lowVoltageVid - highVoltageVid);

	/* Each Vid increment is 12.5 mV.  The minimum slam time is:
	 * vidCodeDelta * 12.5mV * 0.4us/mV
	 * Scale by 100 to avoid decimals.
	 */
	minimumSlamTime = bValue * (125 * 4);

	/* Now round up to nearest register setting.
	 * Note that if we don't find a value, we
	 * will fall through to a value of 7
	 */
	for (bValue = 0; bValue < 7; bValue++) {
		if (minimumSlamTime <= vSlamTimes[bValue])
			break;
	}

	/* Apply the value */
	dtemp = pci_read_config32(dev, 0xD8);
	dtemp &= VSSLAM_MASK;
	dtemp |= bValue;
	pci_write_config32(dev, 0xd8, dtemp);
}

static void config_clk_power_ctrl_reg0(int node) {         
        u32 dword;
      	device_t dev = NODE_PCI(node, 3);
	/* Program fields in Clock Power/Control register0 (F3xD4) */

	/* set F3xD4 Clock Power/Timing Control 0 Register
	 * NbClkDidApplyAll=1b
	 * NbClkDid=100b
	 * PowerStepUp= "platform dependent"
	 * PowerStepDown= "platform dependent"
	 * LinkPllLink=01b
	 * ClkRampHystSel=HW default
	 */
	/* check platform type */
	if (!(get_platform_type() & AMD_PTYPE_SVR)) {
		/* For non-server platform
		 * PowerStepUp=01000b - 50nS
		 * PowerStepDown=01000b - 50ns
		 */
		dword = pci_read_config32(dev, 0xd4);
		dword &= CPTC0_MASK;
		dword |= NB_CLKDID_ALL | NB_CLKDID | PW_STP_UP50 | PW_STP_DN50 | LNK_PLL_LOCK;	/* per BKDG */
		pci_write_config32(dev, 0xd4, dword);
	} else {
		dword = pci_read_config32(dev, 0xd4);
		dword &= CPTC0_MASK;
		/* get number of cores for PowerStepUp & PowerStepDown in server
		   1 core - 400nS  - 0000b
		   2 cores - 200nS - 0010b
		   3 cores - 133nS -> 100nS - 0011b
		   4 cores - 100nS - 0011b
		 */
		switch (get_core_num_in_bsp(node)) {
		case 0:
			dword |= PW_STP_UP400 | PW_STP_DN400;
			break;
		case 1:
		case 2:
			dword |= PW_STP_UP200 | PW_STP_DN200;
			break;
		case 3:
			dword |= PW_STP_UP100 | PW_STP_DN100;
			break;
		default:
			dword |= PW_STP_UP100 | PW_STP_DN100;
			break;
		}
		dword |= NB_CLKDID_ALL | NB_CLKDID | LNK_PLL_LOCK;
		pci_write_config32(dev, 0xd4, dword);
	}
}

static void config_power_ctrl_misc_reg(device_t dev) {
	/* check PVI/SVI */
	u32 dword = pci_read_config32(dev, 0xA0);
	if (dword & PVI_MODE) {	/* PVI */
		/* set slamVidMode to 0 for PVI */
		dword &= VID_SLAM_OFF | PLLLOCK_OFF;
		dword |= PLLLOCK_DFT_L;
		pci_write_config32(dev, 0xA0, dword);
	} else {	/* SVI */
		/* set slamVidMode to 1 for SVI */
		dword &= PLLLOCK_OFF;
		dword |= PLLLOCK_DFT_L | VID_SLAM_ON;
		pci_write_config32(dev, 0xA0, dword);

		u32 dtemp = dword;

		/* Program F3xD8[PwrPlanes] according F3xA0[DulaVdd]  */
		dword = pci_read_config32(dev, 0xD8);

		if (dtemp & DUAL_VDD_BIT)
			dword |= PWR_PLN_ON;
		else
			dword &= PWR_PLN_OFF;
		pci_write_config32(dev, 0xD8, dword);
	}
}

static void prep_fid_change(void)
{
        u32 dword;
	u32 nodes;
	device_t dev;
	int i;

	/* This needs to be run before any Pstate changes are requested */

	nodes = get_nodes();

	for (i = 0; i < nodes; i++) {
		printk(BIOS_DEBUG, "Prep FID/VID Node:%02x \n", i);
		dev = NODE_PCI(i, 3);

		setVSRamp(dev);
		/* BKDG r31116 2010-04-22  2.4.1.7 step b F3xD8[VSSlamTime] */
		/* Figure out the value for VsSlamTime and program it */
		recalculateVsSlamTimeSettingOnCorePre(dev);

		config_clk_power_ctrl_reg0(i);

                config_power_ctrl_misc_reg(dev);
             
		/* Note the following settings are additional from the ported
		 * function setFidVidRegs()
		 */
		dword = pci_read_config32(dev, 0xDc);
		dword |= 0x5 << 12;	/* NbsynPtrAdj set to 0x5 per BKDG (needs reset) */
		pci_write_config32(dev, 0xdc, dword);

		/* Rev B settings - FIXME: support other revs. */
		dword = 0xA0E641E6;
		pci_write_config32(dev, 0x84, dword);

		dword = 0xE600A681;
		pci_write_config32(dev, 0x80, dword);

		dword = pci_read_config32(dev, 0x80);
		printk(BIOS_DEBUG, "  F3x80: %08x \n", dword);
		dword = pci_read_config32(dev, 0x84);
		printk(BIOS_DEBUG, "  F3x84: %08x \n", dword);
		dword = pci_read_config32(dev, 0xD4);
		printk(BIOS_DEBUG, "  F3xD4: %08x \n", dword);
		dword = pci_read_config32(dev, 0xD8);
		printk(BIOS_DEBUG, "  F3xD8: %08x \n", dword);
		dword = pci_read_config32(dev, 0xDC);
		printk(BIOS_DEBUG, "  F3xDC: %08x \n", dword);


	}
}


static void UpdateSinglePlaneNbVid(void)
{
	u32 nbVid, cpuVid;
	u8 i;
	msr_t msr;

	/* copy higher voltage (lower VID) of NBVID & CPUVID to both */
	for (i = 0; i < 5; i++) {
		msr = rdmsr(PS_REG_BASE + i);
		nbVid = (msr.lo & PS_CPU_VID_M_ON) >> PS_CPU_VID_SHFT;
		cpuVid = (msr.lo & PS_NB_VID_M_ON) >> PS_NB_VID_SHFT;

		if (nbVid != cpuVid) {
			if (nbVid > cpuVid)
				nbVid = cpuVid;

			msr.lo = msr.lo & PS_BOTH_VID_OFF;
			msr.lo = msr.lo | (u32) ((nbVid) << PS_NB_VID_SHFT);
			msr.lo = msr.lo | (u32) ((nbVid) << PS_CPU_VID_SHFT);
			wrmsr(PS_REG_BASE + i, msr);
		}
	}
}

static void fixPsNbVidBeforeWR(u32 newNbVid, u32 coreid)
{
	msr_t msr;
	u8 startup_pstate;

	/* This function sets NbVid before the warm reset.
	 *       Get StartupPstate from MSRC001_0071.
	 *       Read Pstate register pionted by [StartupPstate].
	 *       and copy its content to P0 and P1 registers.
	 *       Copy newNbVid to P0[NbVid].
	 *       transition to P1 on all cores,
	 *       then transition to P0 on core 0.
	 *       Wait for MSRC001_0063[CurPstate] = 000b on core 0.
	 */

	msr = rdmsr(0xc0010071);
	startup_pstate = (msr.hi >> (32 - 32)) & 0x07;

	/* Copy startup pstate to P1 and P0 MSRs. Set the maxvid for this node in P0.
	 * Then transition to P1 for corex and P0 for core0.
	 * These setting will be cleared by the warm reset
	 */
	msr = rdmsr(0xC0010064 + startup_pstate);
	wrmsr(0xC0010065, msr);
	wrmsr(0xC0010064, msr);

	msr.lo &= ~0xFE000000;	// clear nbvid
	msr.lo |= newNbVid << 25;
	wrmsr(0xC0010064, msr);

	UpdateSinglePlaneNbVid();

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
}

static void coreDelay(void)
{
	u32 saved;
	u32 hi, lo, msr;
	u32 cycles;

	/* delay ~40us
	   This seems like a hack to me...
	   It would be nice to have a central delay function. */

	cycles = 8000 << 3;	/* x8 (number of 1.25ns ticks) */

	msr = 0x10;		/* TSC */
	_RDMSR(msr, &lo, &hi);
	saved = lo;
	do {
		_RDMSR(msr, &lo, &hi);
	} while (lo - saved < cycles);
}

static void transitionVid(u32 targetVid, u8 dev, u8 isNb)
{
	u32 currentVid, dtemp;
	msr_t msr;
	u8 vsTimecode;
	u16 timeTable[8] = { 10, 20, 30, 40, 60, 100, 200, 500 };
	int vsTime;

	/* This function steps or slam the Nb VID to the target VID.
	 * It uses VSRampTime for [SlamVidMode]=0 ([PviMode]=1)
	 * or VSSlamTime for [SlamVidMode]=1 ([PviMode]=0)to time period.
	 */

	/* get the current VID */
	msr = rdmsr(0xC0010071);
	if (isNb)
		currentVid = (msr.lo >> NB_VID_POS) & BIT_MASK_7;
	else
		currentVid = (msr.lo >> CPU_VID_POS) & BIT_MASK_7;

	/* Read MSRC001_0070 COFVID Control Register */
	msr = rdmsr(0xC0010070);

	/* check PVI/SPI */
	dtemp = pci_read_config32(dev, 0xA0);
	if (dtemp & PVI_MODE) {	/* PVI, step VID */
		if (currentVid < targetVid) {
			while (currentVid < targetVid) {
				currentVid++;
				if (isNb)
					msr.lo = (msr.lo & NB_VID_MASK_OFF) | (currentVid << NB_VID_POS);
				else
					msr.lo = (msr.lo & CPU_VID_MASK_OFF) | (currentVid << CPU_VID_POS);
				wrmsr(0xC0010070, msr);

				/* read F3xD8[VSRampTime]  */
				dtemp = pci_read_config32(dev, 0xD8);
				vsTimecode = (u8) ((dtemp >> VS_RAMP_T) & 0x7);
				vsTime = (int)timeTable[vsTimecode];
				do {
					coreDelay();
					vsTime -= 40;
				} while (vsTime > 0);
			}
		} else if (currentVid > targetVid) {
			while (currentVid > targetVid) {
				currentVid--;
				if (isNb)
					msr.lo = (msr.lo & NB_VID_MASK_OFF) | (currentVid << NB_VID_POS);
				else
					msr.lo = (msr.lo & CPU_VID_MASK_OFF) | (currentVid << CPU_VID_POS);
				wrmsr(0xC0010070, msr);

				/* read F3xD8[VSRampTime]  */
				dtemp = pci_read_config32(dev, 0xD8);
				vsTimecode = (u8) ((dtemp >> VS_RAMP_T) & 0x7);
				vsTime = (int)timeTable[vsTimecode];
				do {
					coreDelay();
					vsTime -= 40;
				} while (vsTime > 0);
			}
		}
	} else {		/* SVI, slam VID */
		if (isNb)
			msr.lo = (msr.lo & NB_VID_MASK_OFF) | (targetVid << NB_VID_POS);
		else
			msr.lo = (msr.lo & CPU_VID_MASK_OFF) | (targetVid << CPU_VID_POS);
		wrmsr(0xC0010070, msr);

		/* read F3xD8[VSRampTime]  */
		dtemp = pci_read_config32(dev, 0xD8);
		vsTimecode = (u8) ((dtemp >> VS_RAMP_T) & 0x7);
		vsTime = (int)timeTable[vsTimecode];
		do {
			coreDelay();
			vsTime -= 40;
		} while (vsTime > 0);
	}
}


static void init_fidvid_ap(u32 bsp_apicid, u32 apicid, u32 nodeid, u32 coreid)
{
	device_t dev;
	u32 vid_max;
	u32 fid_max;
	u8 nb_cof_vid_update;
	u8 pvimode;
	u32 reg1fc;
	u32 send;
	u8 nodes;
	u8 i;

	printk(BIOS_DEBUG, "FIDVID on AP: %02x\n", apicid);

	/* Steps 1-6 of BIOS NB COF and VID Configuration
	 * for SVI and Single-Plane PVI Systems.
	 */

	/* If any node has nb_cof_vid_update set all nodes need an update. */
	nodes = get_nodes();
	nb_cof_vid_update = 0;
	for (i = 0; i < nodes; i++) {
		if (pci_read_config32(NODE_PCI(i, 3), 0x1FC) & 1) {
			nb_cof_vid_update = 1;
			break;
		}
	}

	dev = NODE_PCI(nodeid, 3);
	pvimode = (pci_read_config32(dev, 0xA0) >> 8) & 1;
	reg1fc = pci_read_config32(dev, 0x1FC);

	if (nb_cof_vid_update) {
		if (pvimode) {
			vid_max = (reg1fc >> 7) & 0x7F;
			fid_max = (reg1fc >> 2) & 0x1F;

			/* write newNbVid to P-state Reg's NbVid always if NbVidUpdatedAll=1 */
			fixPsNbVidBeforeWR(vid_max, coreid);
		} else {	/* SVI */
			vid_max = ((reg1fc >> 7) & 0x7F) - ((reg1fc >> 17) & 0x1F);
			fid_max = ((reg1fc >> 2) & 0x1F) + ((reg1fc >> 14) & 0x7);
			transitionVid(vid_max, dev, IS_NB);
		}

		/* fid setup is handled by the BSP at the end. */

	} else {	/* ! nb_cof_vid_update */
		/* Use max values */
		if (pvimode)
			UpdateSinglePlaneNbVid();
	}

	send = (nb_cof_vid_update << 16) | (fid_max << 8);
	send |= (apicid << 24);	// ap apicid

	// Send signal to BSP about this AP max fid
	// This also indicates this AP is ready for warm reset (if required).
	lapic_write(LAPIC_MSG_REG, send | F10_APSTATE_RESET);
}

static u32 calc_common_fid(u32 fid_packed, u32 fid_packed_new)
{
	u32 fidmax;
	u32 fidmax_new;

	fidmax = (fid_packed >> 8) & 0xFF;

	fidmax_new = (fid_packed_new >> 8) & 0xFF;

	if (fidmax > fidmax_new) {
		fidmax = fidmax_new;
	}

	fid_packed &= 0xFF << 16;
	fid_packed |= (fidmax << 8);
	fid_packed |= fid_packed_new & (0xFF << 16);	// set nb_cof_vid_update

	return fid_packed;
}

static void init_fidvid_bsp_stage1(u32 ap_apicid, void *gp)
{
	u32 readback = 0;
	u32 timeout = 1;

	struct fidvid_st *fvp = gp;
	int loop;

	print_debug_fv("Wait for AP stage 1: ap_apicid = ", ap_apicid);

	loop = 100000;
	while (--loop > 0) {
		if (lapic_remote_read(ap_apicid, LAPIC_MSG_REG, &readback) != 0)
			continue;
		if ((readback & 0x3f) == 1) {
			timeout = 0;
			break;	/* target ap is in stage 1 */
		}
	}

	if (timeout) {
		printk(BIOS_DEBUG, "%s: timed out reading from ap %02x\n",
		       __func__, ap_apicid);
		return;
	}

	print_debug_fv("\treadback = ", readback);

	fvp->common_fid = calc_common_fid(fvp->common_fid, readback);

	print_debug_fv("\tcommon_fid(packed) = ", fvp->common_fid);

}

static void updateSviPsNbVidAfterWR(u32 newNbVid)
{
	msr_t msr;
	u8 i;

	/* This function copies newNbVid to NbVid bits in P-state Registers[4:0]
	 * for SVI mode.
	 */

	for (i = 0; i < 5; i++) {
		msr = rdmsr(0xC0010064 + i);
		if ((msr.hi >> 31) & 1) {	/* PstateEn? */
			msr.lo &= ~(0x7F << 25);
			msr.lo |= (newNbVid & 0x7F) << 25;
			wrmsr(0xC0010064 + i, msr);
		}
	}
}


static void fixPsNbVidAfterWR(u32 newNbVid, u8 NbVidUpdatedAll)
{
	msr_t msr;
	u8 i;
	u8 StartupPstate;

	/* This function copies newNbVid to NbVid bits in P-state
	 * Registers[4:0] if its NbDid bit=0 and PstateEn bit =1 in case of
	 * NbVidUpdatedAll =0 or copies copies newNbVid to NbVid bits in
	 * P-state Registers[4:0] if its and PstateEn bit =1 in case of
	 * NbVidUpdatedAll=1. Then transition to StartPstate.
	 */

	/* write newNbVid to P-state Reg's NbVid if its NbDid=0 */
	for (i = 0; i < 5; i++) {
		msr = rdmsr(0xC0010064 + i);
		/*  NbDid (bit 22 of P-state Reg) == 0  or NbVidUpdatedAll = 1 */
		if ((((msr.lo >> 22) & 1) == 0) || NbVidUpdatedAll) {
			msr.lo &= ~(0x7F << 25);
			msr.lo |= (newNbVid & 0x7F) << 25;
			wrmsr(0xC0010064 + i, msr);
		}
	}

	UpdateSinglePlaneNbVid();

	/* For each core in the system, transition all cores to StartupPstate */
	msr = rdmsr(0xC0010071);
	StartupPstate = msr.hi & 0x07;
	msr = rdmsr(0xC0010062);
	msr.lo = StartupPstate;
	wrmsr(0xC0010062, msr);

	/* Wait for StartupPstate to set. */
	do {
		msr = rdmsr(0xC0010063);
	} while (msr.lo != StartupPstate);
}

static void set_p0(void)
{
	msr_t msr;

	// Transition P0 for calling core.
	msr = rdmsr(0xC0010062);
	msr.lo = (msr.lo & ~0x07);
	wrmsr(0xC0010062, msr);

	/* Wait for P0 to set. */
	do {
		msr = rdmsr(0xC0010063);
	} while (msr.lo != 0);
}

static void finalPstateChange(void)
{
	/* Enble P0 on all cores for best performance.
	 * Linux can slow them down later if need be.
	 * It is safe since they will be in C1 halt
	 * most of the time anyway.
	 */
	set_p0();
}

static void init_fidvid_stage2(u32 apicid, u32 nodeid)
{
	msr_t msr;
	device_t dev;
	u32 reg1fc;
	u32 dtemp;
	u32 nbvid;
	u8 nb_cof_vid_update;
	u8 nodes;
	u8 NbVidUpdateAll;
	u8 i;
	u8 pvimode;

	/* After warm reset finish the fid/vid setup for all cores. */

	/* If any node has nb_cof_vid_update set all nodes need an update. */
	nodes = get_nodes();
	nb_cof_vid_update = 0;
	for (i = 0; i < nodes; i++) {
		if (pci_read_config32(NODE_PCI(i, 3), 0x1FC) & 1) {
			nb_cof_vid_update = 1;
			break;
		}
	}

	dev = NODE_PCI(nodeid, 3);
	pvimode = (pci_read_config32(dev, 0xA0) >> 8) & 1;
	reg1fc = pci_read_config32(dev, 0x1FC);
	nbvid = (reg1fc >> 7) & 0x7F;
	NbVidUpdateAll = (reg1fc >> 1) & 1;

	if (nb_cof_vid_update) {
		if (pvimode) {
			nbvid = (reg1fc >> 7) & 0x7F;
			/* write newNbVid to P-state Reg's NbVid if its NbDid=0 */
			fixPsNbVidAfterWR(nbvid, NbVidUpdateAll);
		} else {	/* SVI */
			nbvid = ((reg1fc >> 7) & 0x7F) - ((reg1fc >> 17) & 0x1F);
			updateSviPsNbVidAfterWR(nbvid);
		}
	} else {		/* !nb_cof_vid_update */
		if (pvimode)
			UpdateSinglePlaneNbVid();
	}
	dtemp = pci_read_config32(dev, 0xA0);
	dtemp &= PLLLOCK_OFF;
	dtemp |= PLLLOCK_DFT_L;
	pci_write_config32(dev, 0xA0, dtemp);

	finalPstateChange();

	/* Set TSC to tick at the P0 ndfid rate */
	msr = rdmsr(HWCR);
	msr.lo |= 1 << 24;
	wrmsr(HWCR, msr);
}


#if CONFIG_SET_FIDVID_STORE_AP_APICID_AT_FIRST
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
#if CONFIG_SET_FIDVID_STORE_AP_APICID_AT_FIRST
	struct ap_apicid_st ap_apicidx;
	u32 i;
#endif
	struct fidvid_st fv;
	device_t dev;
	u32 vid_max;
	u32 fid_max=0;
	u8 nb_cof_vid_update;
	u32 reg1fc;
	u8 pvimode;

	printk(BIOS_DEBUG, "FIDVID on BSP, APIC_id: %02x\n", bsp_apicid);
	/* FIXME: The first half of this function is nearly the same as
	 * init_fidvid_bsp() and the code could be combined.
	 */

	/* Steps 1-6 of BIOS NB COF and VID Configuration
	 * for SVI and Single-Plane PVI Systems.
	 */

	/* If any node has nb_cof_vid_update set all nodes need an update. */
	nb_cof_vid_update = 0;
	for (i = 0; i < nodes; i++) {
		if (pci_read_config32(NODE_PCI(i, 3), 0x1FC) & 1) {
			nb_cof_vid_update = 1;
			break;
		}
	}

	dev = NODE_PCI(0, 3);
	pvimode = (pci_read_config32(dev, 0xA0) >> 8) & 1;
	reg1fc = pci_read_config32(dev, 0x1FC);

	if (nb_cof_vid_update) {
		if (pvimode) {
			vid_max = (reg1fc >> 7) & 0x7F;
			fid_max = (reg1fc >> 2) & 0x1F;

			/* write newNbVid to P-state Reg's NbVid always if NbVidUpdatedAll=1 */
			fixPsNbVidBeforeWR(vid_max, 0);
		} else {	/* SVI */
			vid_max = ((reg1fc >> 7) & 0x7F) - ((reg1fc >> 17) & 0x1F);
			fid_max = ((reg1fc >> 2) & 0x1F) + ((reg1fc >> 14) & 0x7);
			transitionVid(vid_max, dev, IS_NB);
		}

		/*  fid setup is handled by the BSP at the end. */

	} else {		/* ! nb_cof_vid_update */
		/* Use max values */
		if (pvimode)
			UpdateSinglePlaneNbVid();
	}

	fv.common_fid = (nb_cof_vid_update << 16) | (fid_max << 8);
	print_debug_fv("BSP fid = ", fv.common_fid);

#if CONFIG_SET_FIDVID_STORE_AP_APICID_AT_FIRST && !CONFIG_SET_FIDVID_CORE0_ONLY
	/* For all APs (We know the APIC ID of all APs even when the APIC ID
	   is lifted) remote read from AP LAPIC_MSG_REG about max fid.
	   Then calculate the common max fid that can be used for all
	   APs and BSP */
	ap_apicidx.num = 0;

	for_each_ap(bsp_apicid, CONFIG_SET_FIDVID_CORE_RANGE, store_ap_apicid, &ap_apicidx);

	for (i = 0; i < ap_apicidx.num; i++) {
		init_fidvid_bsp_stage1(ap_apicidx.apicid[i], &fv);
	}
#else
	for_each_ap(bsp_apicid, CONFIG_SET_FIDVID_CORE0_ONLY, init_fidvid_bsp_stage1, &fv);
#endif

	print_debug_fv("common_fid = ", fv.common_fid);

	if (fv.common_fid & (1 << 16)) {	/* check nb_cof_vid_update */

		// Enable the common fid and other settings.
		enable_fid_change((fv.common_fid >> 8) & 0x1F);

		// nbfid change need warm reset, so reset at first
		return 1;
	}

	return 0;		// No FID/VID changes. Don't reset
}
#endif
