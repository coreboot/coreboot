/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Advanced Micro Devices, Inc.
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

#include "defaults.h"
#include <stdlib.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mtrr.h>
#include <northbridge/amd/amdfam10/amdfam10.h>
#include <northbridge/amd/amdht/AsPsDefs.h>
#include <northbridge/amd/amdht/porting.h>

#include <cpu/x86/mtrr/earlymtrr.c>
#include <northbridge/amd/amdfam10/raminit_amdmct.c>
#include <reset.h>

static void prep_fid_change(void);
static void init_fidvid_stage2(u32 apicid, u32 nodeid);
void cpuSetAMDMSR(void);

#if CONFIG_PCI_IO_CFG_EXT
static void set_EnableCf8ExtCfg(void)
{
	// set the NB_CFG[46]=1;
	msr_t msr;
	msr = rdmsr(NB_CFG_MSR);
	// EnableCf8ExtCfg: We need that to access CONFIG_PCI_IO_CFG_EXT 4K range
	msr.hi |= (1 << (46 - 32));
	wrmsr(NB_CFG_MSR, msr);
}
#else
static void set_EnableCf8ExtCfg(void) { }
#endif


typedef void (*process_ap_t) (u32 apicid, void *gp);

//core_range = 0 : all cores
//core range = 1 : core 0 only
//core range = 2 : cores other than core0

static void for_each_ap(u32 bsp_apicid, u32 core_range, process_ap_t process_ap,
			void *gp)
{
	// here assume the OS don't change our apicid
	u32 ap_apicid;

	u32 nodes;
	u32 siblings;
	u32 disable_siblings;
	u32 cores_found;
	u32 nb_cfg_54;
	int i, j;
	u32 ApicIdCoreIdSize;

	/* get_nodes define in ht_wrapper.c */
	nodes = get_nodes();

	if (!CONFIG_LOGICAL_CPUS ||
	    read_option(multi_core, 0) != 0) {	// 0 means multi core
		disable_siblings = 1;
	} else {
		disable_siblings = 0;
	}

	/* Assume that all node are same stepping, otherwise we can use use
	   nb_cfg_54 from bsp for all nodes */
	nb_cfg_54 = read_nb_cfg_54();

	ApicIdCoreIdSize = (cpuid_ecx(0x80000008) >> 12 & 0xf);
	if (ApicIdCoreIdSize) {
		siblings = ((1 << ApicIdCoreIdSize) - 1);
	} else {
		siblings = 3;	//quad core
	}

	for (i = 0; i < nodes; i++) {
		cores_found = get_core_num_in_bsp(i);

		u32 jstart, jend;

		if (core_range == 2) {
			jstart = 1;
		} else {
			jstart = 0;
		}

		if (disable_siblings || (core_range == 1)) {
			jend = 0;
		} else {
			jend = cores_found;
		}

		for (j = jstart; j <= jend; j++) {
			ap_apicid =
			    i * (nb_cfg_54 ? (siblings + 1) : 1) +
			    j * (nb_cfg_54 ? 1 : 64);

#if CONFIG_ENABLE_APIC_EXT_ID && (CONFIG_APIC_ID_OFFSET > 0)
#if !CONFIG_LIFT_BSP_APIC_ID
			if ((i != 0) || (j != 0))	/* except bsp */
#endif
				ap_apicid += CONFIG_APIC_ID_OFFSET;
#endif

			if (ap_apicid == bsp_apicid)
				continue;

			process_ap(ap_apicid, gp);

		}
	}
}

static inline int lapic_remote_read(int apicid, int reg, u32 *pvalue)
{
	int timeout;
	u32 status;
	int result;
	lapic_wait_icr_idle();
	lapic_write(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(apicid));
	lapic_write(LAPIC_ICR, LAPIC_DM_REMRD | (reg >> 4));

/* Extra busy check compared to lapic.h */
	timeout = 0;
	do {
		status = lapic_read(LAPIC_ICR) & LAPIC_ICR_BUSY;
	} while (status == LAPIC_ICR_BUSY && timeout++ < 1000);

	timeout = 0;
	do {
		status = lapic_read(LAPIC_ICR) & LAPIC_ICR_RR_MASK;
	} while (status == LAPIC_ICR_RR_INPROG && timeout++ < 1000);

	result = -1;

	if (status == LAPIC_ICR_RR_VALID) {
		*pvalue = lapic_read(LAPIC_RRR);
		result = 0;
	}
	return result;
}

#if CONFIG_SET_FIDVID
static void init_fidvid_ap(u32 apicid, u32 nodeid, u32 coreid);
#endif

static inline __attribute__ ((always_inline))
void print_apicid_nodeid_coreid(u32 apicid, struct node_core_id id,
				const char *str)
{
	printk(BIOS_DEBUG,
	       "%s --- { APICID = %02x NODEID = %02x COREID = %02x} ---\n", str,
	       apicid, id.nodeid, id.coreid);
}

static u32 wait_cpu_state(u32 apicid, u32 state)
{
	u32 readback = 0;
	u32 timeout = 1;
	int loop = 4000000;
	while (--loop > 0) {
		if (lapic_remote_read(apicid, LAPIC_MSG_REG, &readback) != 0)
			continue;
		if ((readback & 0x3f) == state || (readback & 0x3f) == F10_APSTATE_RESET) {
			timeout = 0;
			break;	//target cpu is in stage started
		}
	}
	if (timeout) {
		if (readback) {
			timeout = readback;
		}
	}

	return timeout;
}

static void wait_ap_started(u32 ap_apicid, void *gp)
{
	u32 timeout;
	timeout = wait_cpu_state(ap_apicid, F10_APSTATE_STARTED);
	printk(BIOS_DEBUG, "* AP %02x", ap_apicid);
	if (timeout) {
		printk(BIOS_DEBUG, " timed out:%08x\n", timeout);
	} else {
		printk(BIOS_DEBUG, "started\n");
	}
}

void wait_all_other_cores_started(u32 bsp_apicid)
{
	// all aps other than core0
	printk(BIOS_DEBUG, "started ap apicid: ");
	for_each_ap(bsp_apicid, 2, wait_ap_started, (void *)0);
	printk(BIOS_DEBUG, "\n");
}

void allow_all_aps_stop(u32 bsp_apicid)
{
	/* Called by the BSP to indicate AP can stop */

	/* FIXME Do APs use this? */

	// allow aps to stop use 6 bits for state
	lapic_write(LAPIC_MSG_REG, (bsp_apicid << 24) | F10_APSTATE_STOPPED);
}

static void enable_apic_ext_id(u32 node)
{
	u32 val;

	val = pci_read_config32(NODE_HT(node), 0x68);
	val |= (HTTC_APIC_EXT_SPUR | HTTC_APIC_EXT_ID | HTTC_APIC_EXT_BRD_CST);
	pci_write_config32(NODE_HT(node), 0x68, val);
}

static void STOP_CAR_AND_CPU(void)
{
	msr_t msr;

	/* Disable L2 IC to L3 connection (Only for CAR) */
	msr = rdmsr(BU_CFG2);
	msr.lo &= ~(1 << ClLinesToNbDis);
	wrmsr(BU_CFG2, msr);

	disable_cache_as_ram();	// inline
	/* stop all cores except node0/core0 the bsp .... */
	stop_this_cpu();
}

#if CONFIG_RAMINIT_SYSINFO
static u32 init_cpus(u32 cpu_init_detectedx, struct sys_info *sysinfo)
#else
static u32 init_cpus(u32 cpu_init_detectedx)
#endif
{
	u32 bsp_apicid = 0;
	u32 apicid;
	struct node_core_id id;

	/*
	 * already set early mtrr in cache_as_ram.inc
	 */

	/* that is from initial apicid, we need nodeid and coreid
	   later */
	id = get_node_core_id_x();

	/* NB_CFG MSR is shared between cores, so we need make sure
	   core0 is done at first --- use wait_all_core0_started  */
	if (id.coreid == 0) {
		set_apicid_cpuid_lo();	/* only set it on core0 */
		set_EnableCf8ExtCfg();	/* only set it on core0 */
#if CONFIG_ENABLE_APIC_EXT_ID
		enable_apic_ext_id(id.nodeid);
#endif
	}

	enable_lapic();

#if CONFIG_ENABLE_APIC_EXT_ID && (CONFIG_APIC_ID_OFFSET > 0)
	u32 initial_apicid = get_initial_apicid();

#if !CONFIG_LIFT_BSP_APIC_ID
	if (initial_apicid != 0)	// other than bsp
#endif
	{
		/* use initial apic id to lift it */
		u32 dword = lapic_read(LAPIC_ID);
		dword &= ~(0xff << 24);
		dword |=
		    (((initial_apicid + CONFIG_APIC_ID_OFFSET) & 0xff) << 24);

		lapic_write(LAPIC_ID, dword);
	}
#if CONFIG_LIFT_BSP_APIC_ID
	bsp_apicid += CONFIG_APIC_ID_OFFSET;
#endif

#endif

	/* get the apicid, it may be lifted already */
	apicid = lapicid();

	// show our apicid, nodeid, and coreid
	if (id.coreid == 0) {
		if (id.nodeid != 0)	//all core0 except bsp
			print_apicid_nodeid_coreid(apicid, id, " core0: ");
	} else {		//all other cores
		print_apicid_nodeid_coreid(apicid, id, " corex: ");
	}

	if (cpu_init_detectedx) {
		print_apicid_nodeid_coreid(apicid, id,
					   "\n\n\nINIT detected from ");
		printk(BIOS_DEBUG, "\nIssuing SOFT_RESET...\n");
		soft_reset();
	}

	if (id.coreid == 0) {
		if (!(warm_reset_detect(id.nodeid)))	//FIXME: INIT is checked above but check for more resets?
			distinguish_cpu_resets(id.nodeid);	// Also indicates we are started
	}
	// Mark the core as started.
	lapic_write(LAPIC_MSG_REG, (apicid << 24) | F10_APSTATE_STARTED);

	if (apicid != bsp_apicid) {
		/* Setup each AP's cores MSRs.
		 * This happens after HTinit.
		 * The BSP runs this code in it's own path.
		 */
#if CONFIG_UPDATE_CPU_MICROCODE
		update_microcode(cpuid_eax(1));
#endif
		cpuSetAMDMSR();

#if CONFIG_SET_FIDVID
#if CONFIG_LOGICAL_CPUS && CONFIG_SET_FIDVID_CORE0_ONLY
		// Run on all AP for proper FID/VID setup.
		if (id.coreid == 0)	// only need set fid for core0
#endif
		{
			// check warm(bios) reset to call stage2 otherwise do stage1
			if (warm_reset_detect(id.nodeid)) {
				printk(BIOS_DEBUG,
				       "init_fidvid_stage2 apicid: %02x\n",
				       apicid);
				init_fidvid_stage2(apicid, id.nodeid);
			} else {
				printk(BIOS_DEBUG,
				       "init_fidvid_ap(stage1) apicid: %02x\n",
				       apicid);
				init_fidvid_ap(apicid, id.nodeid, id.coreid);
			}
		}
#endif

		/* AP is ready, configure MTRRs and go to sleep */
		set_var_mtrr(0, 0x00000000, CONFIG_RAMTOP, MTRR_TYPE_WRBACK);

		STOP_CAR_AND_CPU();

		printk(BIOS_DEBUG,
		       "\nAP %02x should be halted but you are reading this....\n",
		       apicid);
	}

	return bsp_apicid;
}

static u32 is_core0_started(u32 nodeid)
{
	u32 htic;
	device_t device;
	device = NODE_PCI(nodeid, 0);
	htic = pci_read_config32(device, HT_INIT_CONTROL);
	htic &= HTIC_ColdR_Detect;
	return htic;
}

void wait_all_core0_started(void)
{
	/* When core0 is started, it will distingush_cpu_resets
	 * So wait for that to finish */
	u32 i;
	u32 nodes = get_nodes();

	printk(BIOS_DEBUG, "core0 started: ");
	for (i = 1; i < nodes; i++) {	// skip bsp, because it is running on bsp
		while (!is_core0_started(i)) {
		}
		printk(BIOS_DEBUG, " %02x", i);
	}
	printk(BIOS_DEBUG, "\n");
}

#if CONFIG_MAX_PHYSICAL_CPUS > 1
/**
 * void start_node(u32 node)
 *
 *  start the core0 in node, so it can generate HT packet to feature code.
 *
 * This function starts the AP nodes core0s. wait_all_core0_started() in
 * romstage.c waits for all the AP to be finished before continuing
 * system init.
 */
static void start_node(u8 node)
{
	u32 val;

	/* Enable routing table */
	printk(BIOS_DEBUG, "Start node %02x", node);

#if CONFIG_NORTHBRIDGE_AMD_AMDFAM10
	/* For FAM10 support, we need to set Dram base/limit for the new node */
	pci_write_config32(NODE_MP(node), 0x44, 0);
	pci_write_config32(NODE_MP(node), 0x40, 3);
#endif

	/* Allow APs to make requests (ROM fetch) */
	val = pci_read_config32(NODE_HT(node), 0x6c);
	val &= ~(1 << 1);
	pci_write_config32(NODE_HT(node), 0x6c, val);

	printk(BIOS_DEBUG, " done.\n");
}

/**
 * static void setup_remote_node(u32 node)
 *
 * Copy the BSP Address Map to each AP.
 */
static void setup_remote_node(u8 node)
{
	/* There registers can be used with F1x114_x Address Map at the
	   same time, So must set them even 32 node */
	static const u16 pci_reg[] = {
		/* DRAM Base/Limits Registers */
		0x44, 0x4c, 0x54, 0x5c, 0x64, 0x6c, 0x74, 0x7c,
		0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78,
		0x144, 0x14c, 0x154, 0x15c, 0x164, 0x16c, 0x174, 0x17c,
		0x140, 0x148, 0x150, 0x158, 0x160, 0x168, 0x170, 0x178,
		/* MMIO Base/Limits Registers */
		0x84, 0x8c, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbc,
		0x80, 0x88, 0x90, 0x98, 0xa0, 0xa8, 0xb0, 0xb8,
		/* IO Base/Limits Registers */
		0xc4, 0xcc, 0xd4, 0xdc,
		0xc0, 0xc8, 0xd0, 0xd8,
		/* Configuration Map Registers */
		0xe0, 0xe4, 0xe8, 0xec,
	};
	u16 i;

	printk(BIOS_DEBUG, "setup_remote_node: %02x", node);

	/* copy the default resource map from node 0 */
	for (i = 0; i < ARRAY_SIZE(pci_reg); i++) {
		u32 value;
		u16 reg;
		reg = pci_reg[i];
		value = pci_read_config32(NODE_MP(0), reg);
		pci_write_config32(NODE_MP(node), reg, value);

	}
	printk(BIOS_DEBUG, " done\n");
}
#endif				/* CONFIG_MAX_PHYSICAL_CPUS > 1 */

static void AMD_Errata281(u8 node, u32 revision, u32 platform)
{
	/* Workaround for Transaction Scheduling Conflict in
	 * Northbridge Cross Bar.  Implement XCS Token adjustment
	 * for ganged links.  Also, perform fix up for the mixed
	 * revision case.
	 */

	u32 reg, val;
	u8 i;
	u8 mixed = 0;
	u8 nodes = get_nodes();

	if (platform & AMD_PTYPE_SVR) {
		/* For each node we need to check for a "broken" node */
		if (!(revision & (AMD_DR_B0 | AMD_DR_B1))) {
			for (i = 0; i < nodes; i++) {
				if (mctGetLogicalCPUID(i) &
				    (AMD_DR_B0 | AMD_DR_B1)) {
					mixed = 1;
					break;
				}
			}
		}

		if ((revision & (AMD_DR_B0 | AMD_DR_B1)) || mixed) {

			/* F0X68[22:21] DsNpReqLmt0 = 01b */
			val = pci_read_config32(NODE_PCI(node, 0), 0x68);
			val &= ~0x00600000;
			val |= 0x00200000;
			pci_write_config32(NODE_PCI(node, 0), 0x68, val);

			/* F3X6C */
			val = pci_read_config32(NODE_PCI(node, 3), 0x6C);
			val &= ~0x700780F7;
			val |= 0x00010094;
			pci_write_config32(NODE_PCI(node, 3), 0x6C, val);

			/* F3X7C */
			val = pci_read_config32(NODE_PCI(node, 3), 0x7C);
			val &= ~0x707FFF1F;
			val |= 0x00144514;
			pci_write_config32(NODE_PCI(node, 3), 0x7C, val);

			/* F3X144[3:0] RspTok = 0001b */
			val = pci_read_config32(NODE_PCI(node, 3), 0x144);
			val &= ~0x0000000F;
			val |= 0x00000001;
			pci_write_config32(NODE_PCI(node, 3), 0x144, val);

			for (i = 0; i < 3; i++) {
				reg = 0x148 + (i * 4);
				val = pci_read_config32(NODE_PCI(node, 3), reg);
				val &= ~0x000000FF;
				val |= 0x000000DB;
				pci_write_config32(NODE_PCI(node, 3), reg, val);
			}
		}
	}
}

static void AMD_Errata298(void)
{
	/* Workaround for L2 Eviction May Occur during operation to
	 * set Accessed or dirty bit.
	 */

	msr_t msr;
	u8 i;
	u8 affectedRev = 0;
	u8 nodes = get_nodes();

	/* For each core we need to check for a "broken" node */
	for (i = 0; i < nodes; i++) {
		if (mctGetLogicalCPUID(i) & (AMD_DR_B0 | AMD_DR_B1 | AMD_DR_B2)) {
			affectedRev = 1;
			break;
		}
	}

	if (affectedRev) {
		msr = rdmsr(HWCR);
		msr.lo |= 0x08;	/* Set TlbCacheDis bit[3] */
		wrmsr(HWCR, msr);

		msr = rdmsr(BU_CFG);
		msr.lo |= 0x02;	/* Set TlbForceMemTypeUc bit[1] */
		wrmsr(BU_CFG, msr);

		msr = rdmsr(OSVW_ID_Length);
		msr.lo |= 0x01;	/* OS Visible Workaround - MSR */
		wrmsr(OSVW_ID_Length, msr);

		msr = rdmsr(OSVW_Status);
		msr.lo |= 0x01;	/* OS Visible Workaround - MSR */
		wrmsr(OSVW_Status, msr);
	}

	if (!affectedRev && (mctGetLogicalCPUID(0xFF) & AMD_DR_B3)) {
		msr = rdmsr(OSVW_ID_Length);
		msr.lo |= 0x01;	/* OS Visible Workaround - MSR */
		wrmsr(OSVW_ID_Length, msr);

	}
}

static u32 get_platform_type(void)
{
	u32 ret = 0;

	switch (SYSTEM_TYPE) {
	case 1:
		ret |= AMD_PTYPE_DSK;
		break;
	case 2:
		ret |= AMD_PTYPE_MOB;
		break;
	case 0:
		ret |= AMD_PTYPE_SVR;
		break;
	default:
		break;
	}

	/* FIXME: add UMA support. */

	/* All Fam10 are multi core */
	ret |= AMD_PTYPE_MC;

	return ret;
}

static void AMD_SetupPSIVID_d(u32 platform_type, u8 node)
{
	u32 dword;
	int i;
	msr_t msr;

	if (platform_type & (AMD_PTYPE_MOB | AMD_PTYPE_DSK)) {

		/* The following code sets the PSIVID to the lowest support P state
		 * assuming that the VID for the lowest power state is below
		 * the VDD voltage regulator threshold. (This also assumes that there
		 * is a Pstate lower than P0)
		 */

		for (i = 4; i >= 0; i--) {
			msr = rdmsr(PS_REG_BASE + i);
			/*  Pstate valid? */
			if (msr.hi & PS_EN_MASK) {
				dword = pci_read_config32(NODE_PCI(i, 3), 0xA0);
				dword &= ~0x7F;
				dword |= (msr.lo >> 9) & 0x7F;
				pci_write_config32(NODE_PCI(i, 3), 0xA0, dword);
				break;
			}
		}
	}
}

/**
 * AMD_CpuFindCapability - Traverse PCI capability list to find host HT links.
 *  HT Phy operations are not valid on links that aren't present, so this
 *  prevents invalid accesses.
 *
 * Returns the offset of the link register.
 */
static BOOL AMD_CpuFindCapability(u8 node, u8 cap_count, u8 * offset)
{
	u32 reg;
	u32 val;

	/* get start of CPU HT Host Capabilities */
	val = pci_read_config32(NODE_PCI(node, 0), 0x34);
	val &= 0xFF;		//reg offset of first link

	cap_count++;

	/* Traverse through the capabilities. */
	do {
		reg = pci_read_config32(NODE_PCI(node, 0), val);
		/* Is the capability block a HyperTransport capability block? */
		if ((reg & 0xFF) == 0x08) {
			/* Is the HT capability block an HT Host Capability? */
			if ((reg & 0xE0000000) == (1 << 29))
				cap_count--;
		}

		if (cap_count)
			val = (reg >> 8) & 0xFF;	//update reg offset
	} while (cap_count && val);

	*offset = (u8) val;

	/* If requested capability found val != 0 */
	if (!cap_count)
		return TRUE;
	else
		return FALSE;
}

/**
 * AMD_checkLinkType - Compare desired link characteristics using a logical
 *     link type mask.
 *
 * Returns the link characteristic mask.
 */
static u32 AMD_checkLinkType(u8 node, u8 link, u8 regoff)
{
	u32 val;
	u32 linktype = 0;

	/* Check connect, init and coherency */
	val = pci_read_config32(NODE_PCI(node, 0), regoff + 0x18);
	val &= 0x1F;

	if (val == 3)
		linktype |= HTPHY_LINKTYPE_COHERENT;

	if (val == 7)
		linktype |= HTPHY_LINKTYPE_NONCOHERENT;

	if (linktype) {
		/* Check gen3 */
		val = pci_read_config32(NODE_PCI(node, 0), regoff + 0x08);

		if (((val >> 8) & 0x0F) > 6)
			linktype |= HTPHY_LINKTYPE_HT3;
		else
			linktype |= HTPHY_LINKTYPE_HT1;

		/* Check ganged */
		val = pci_read_config32(NODE_PCI(node, 0), (link << 2) + 0x170);

		if (val & 1)
			linktype |= HTPHY_LINKTYPE_GANGED;
		else
			linktype |= HTPHY_LINKTYPE_UNGANGED;
	}
	return linktype;
}

/**
 * AMD_SetHtPhyRegister - Use the HT link's HT Phy portal registers to update
 *   a phy setting for that link.
 */
static void AMD_SetHtPhyRegister(u8 node, u8 link, u8 entry)
{
	u32 phyReg;
	u32 phyBase;
	u32 val;

	/* Determine this link's portal */
	if (link > 3)
		link -= 4;

	phyBase = ((u32) link << 3) | 0x180;

	/* Get the portal control register's initial value
	 * and update it to access the desired phy register
	 */
	phyReg = pci_read_config32(NODE_PCI(node, 4), phyBase);

	if (fam10_htphy_default[entry].htreg > 0x1FF) {
		phyReg &= ~HTPHY_DIRECT_OFFSET_MASK;
		phyReg |= HTPHY_DIRECT_MAP;
	} else {
		phyReg &= ~HTPHY_OFFSET_MASK;
	}

	/* Now get the current phy register data
	 * LinkPhyDone = 0, LinkPhyWrite = 0 is a read
	 */
	phyReg |= fam10_htphy_default[entry].htreg;
	pci_write_config32(NODE_PCI(node, 4), phyBase, phyReg);

	do {
		val = pci_read_config32(NODE_PCI(node, 4), phyBase);
	} while (!(val & HTPHY_IS_COMPLETE_MASK));

	/* Now we have the phy register data, apply the change */
	val = pci_read_config32(NODE_PCI(node, 4), phyBase + 4);
	val &= ~fam10_htphy_default[entry].mask;
	val |= fam10_htphy_default[entry].data;
	pci_write_config32(NODE_PCI(node, 4), phyBase + 4, val);

	/* write it through the portal to the phy
	 * LinkPhyDone = 0, LinkPhyWrite = 1 is a write
	 */
	phyReg |= HTPHY_WRITE_CMD;
	pci_write_config32(NODE_PCI(node, 4), phyBase, phyReg);

	do {
		val = pci_read_config32(NODE_PCI(node, 4), phyBase);
	} while (!(val & HTPHY_IS_COMPLETE_MASK));
}

void cpuSetAMDMSR(void)
{
	/* This routine loads the CPU with default settings in fam10_msr_default
	 * table . It must be run after Cache-As-RAM has been enabled, and
	 * Hypertransport initialization has taken place.  Also note
	 * that it is run on the current processor only, and only for the current
	 * processor core.
	 */
	msr_t msr;
	u8 i;
	u32 revision, platform;

	printk(BIOS_DEBUG, "cpuSetAMDMSR ");

	revision = mctGetLogicalCPUID(0xFF);
	platform = get_platform_type();

	for (i = 0; i < ARRAY_SIZE(fam10_msr_default); i++) {
		if ((fam10_msr_default[i].revision & revision) &&
		    (fam10_msr_default[i].platform & platform)) {
			msr = rdmsr(fam10_msr_default[i].msr);
			msr.hi &= ~fam10_msr_default[i].mask_hi;
			msr.hi |= fam10_msr_default[i].data_hi;
			msr.lo &= ~fam10_msr_default[i].mask_lo;
			msr.lo |= fam10_msr_default[i].data_lo;
			wrmsr(fam10_msr_default[i].msr, msr);
		}
	}
	AMD_Errata298();

	printk(BIOS_DEBUG, " done\n");
}

static void cpuSetAMDPCI(u8 node)
{
	/* This routine loads the CPU with default settings in fam10_pci_default
	 * table . It must be run after Cache-As-RAM has been enabled, and
	 * Hypertransport initialization has taken place.  Also note
	 * that it is run for the first core on each node
	 */
	u8 i, j;
	u32 revision, platform;
	u32 val;
	u8 offset;

	printk(BIOS_DEBUG, "cpuSetAMDPCI %02d", node);

	revision = mctGetLogicalCPUID(node);
	platform = get_platform_type();

	AMD_SetupPSIVID_d(platform, node);	/* Set PSIVID offset which is not table driven */

	for (i = 0; i < ARRAY_SIZE(fam10_pci_default); i++) {
		if ((fam10_pci_default[i].revision & revision) &&
		    (fam10_pci_default[i].platform & platform)) {
			val = pci_read_config32(NODE_PCI(node,
							 fam10_pci_default[i].
							 function),
						fam10_pci_default[i].offset);
			val &= ~fam10_pci_default[i].mask;
			val |= fam10_pci_default[i].data;
			pci_write_config32(NODE_PCI(node,
						    fam10_pci_default[i].
						    function),
					   fam10_pci_default[i].offset, val);
		}
	}

	for (i = 0; i < ARRAY_SIZE(fam10_htphy_default); i++) {
		if ((fam10_htphy_default[i].revision & revision) &&
		    (fam10_htphy_default[i].platform & platform)) {
			/* HT Phy settings either apply to both sublinks or have
			 * separate registers for sublink zero and one, so there
			 * will be two table entries. So, here we only loop
			 * through the sublink zeros in function zero.
			 */
			for (j = 0; j < 4; j++) {
				if (AMD_CpuFindCapability(node, j, &offset)) {
					if (AMD_checkLinkType(node, j, offset)
					    & fam10_htphy_default[i].linktype) {
						AMD_SetHtPhyRegister(node, j,
								     i);
					}
				} else {
					/* No more capabilities,
					 * link not present
					 */
					break;
				}
			}
		}
	}

	/* FIXME: add UMA support and programXbarToSriReg(); */

	AMD_Errata281(node, revision, platform);

	/* FIXME: if the dct phy doesn't init correct it needs to reset.
	   if (revision & (AMD_DR_B2 | AMD_DR_B3))
	   dctPhyDiag(); */

	printk(BIOS_DEBUG, " done\n");
}

#ifdef UNUSED_CODE
static void cpuInitializeMCA(void)
{
	/* Clears Machine Check Architecture (MCA) registers, which power on
	 * containing unknown data, on currently running processor.
	 * This routine should only be executed on initial power on (cold boot),
	 * not across a warm reset because valid data is present at that time.
	 */

	msr_t msr;
	u32 reg;
	u8 i;

	if (cpuid_edx(1) & 0x4080) {	/* MCE and MCA (edx[7] and edx[14]) */
		msr = rdmsr(MCG_CAP);
		if (msr.lo & MCG_CTL_P) {	/* MCG_CTL_P bit is set? */
			msr.lo &= 0xFF;
			msr.lo--;
			msr.lo <<= 2;	/* multiply the count by 4 */
			reg = MC0_STA + msr.lo;
			msr.lo = msr.hi = 0;
			for (i = 0; i < 4; i++) {
				wrmsr(reg, msr);
				reg -= 4;	/* Touch status regs for each bank */
			}
		}
	}
}
#endif

/**
 * finalize_node_setup()
 *
 * Do any additional post HT init
 *
 */
static void finalize_node_setup(struct sys_info *sysinfo)
{
	u8 i;
	u8 nodes = get_nodes();

#if CONFIG_RAMINIT_SYSINFO
	u32 reg;
	/* read Node0 F0_0x64 bit [8:10] to find out SbLink # */
	reg = pci_read_config32(NODE_HT(0), 0x64);
	sysinfo->sblk = (reg >> 8) & 7;
	sysinfo->sbbusn = 0;
	sysinfo->nodes = nodes;
	sysinfo->sbdn = get_sbdn(sysinfo->sbbusn);
#endif

	for (i = 0; i < nodes; i++) {
		cpuSetAMDPCI(i);
	}

#if CONFIG_SET_FIDVID
	// Prep each node for FID/VID setup.
	prep_fid_change();
#endif

#if CONFIG_MAX_PHYSICAL_CPUS > 1
	/* Skip the BSP, start at node 1 */
	for (i = 1; i < nodes; i++) {
		setup_remote_node(i);
		start_node(i);
	}
#endif
}

#include "fidvid.c"
