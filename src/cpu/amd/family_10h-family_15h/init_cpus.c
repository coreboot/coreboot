/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <compiler.h>
#include "init_cpus.h"

#if IS_ENABLED(CONFIG_HAVE_OPTION_TABLE)
#include "option_table.h"
#endif
#include <pc80/mc146818rtc.h>

#include <northbridge/amd/amdht/ht_wrapper.h>
#include <northbridge/amd/amdht/AsPsDefs.h>
#include <northbridge/amd/amdht/porting.h>
#include <northbridge/amd/amdht/h3ncmn.h>

#if IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_SB700)
#include <southbridge/amd/sb700/sb700.h>
#endif

#if IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_SB800)
#include <southbridge/amd/sb800/sb800.h>
#endif

#include "cpu/amd/car/disable_cache_as_ram.c"

#if IS_ENABLED(CONFIG_PCI_IO_CFG_EXT)
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

// #define DEBUG_HT_SETUP 1
// #define FAM10_AP_NODE_SEQUENTIAL_START 1

uint32_t get_boot_apic_id(uint8_t node, uint32_t core) {
	uint32_t ap_apicid;

	uint32_t nb_cfg_54;
	uint32_t siblings;
	uint32_t cores_found;

	uint8_t fam15h = 0;
	uint8_t rev_gte_d = 0;
	uint8_t dual_node = 0;
	uint32_t f3xe8;
	uint32_t family;
	uint32_t model;

	uint32_t ApicIdCoreIdSize;

	/* Assume that all node are same stepping, otherwise we can use use
	   nb_cfg_54 from bsp for all nodes */
	nb_cfg_54 = read_nb_cfg_54();
	f3xe8 = pci_read_config32(NODE_PCI(0, 3), 0xe8);

	family = model = cpuid_eax(0x80000001);
	model = ((model & 0xf0000) >> 12) | ((model & 0xf0) >> 4);
	family = ((family & 0xf00000) >> 16) | ((family & 0xf00) >> 8);

	if (family >= 0x6f) {
		/* Family 15h or later */
		fam15h = 1;
		nb_cfg_54 = 1;
	}

	if ((model >= 0x8) || fam15h)
		/* Revision D or later */
		rev_gte_d = 1;

	if (rev_gte_d)
		 /* Check for dual node capability */
		if (f3xe8 & 0x20000000)
			dual_node = 1;

	ApicIdCoreIdSize = (cpuid_ecx(0x80000008) >> 12 & 0xf);
	if (ApicIdCoreIdSize) {
		siblings = ((1 << ApicIdCoreIdSize) - 1);
	} else {
		siblings = 3;	//quad core
	}

	cores_found = get_core_num_in_bsp(node);
	if (siblings > cores_found)
		siblings = cores_found;

	if (dual_node) {
		ap_apicid = 0;
		if (fam15h) {
			ap_apicid |= ((node >> 1) & 0x3) << 5;			/* Node ID */
			ap_apicid |= ((node & 0x1) * (siblings + 1)) + core;	/* Core ID */
		} else {
			if (nb_cfg_54) {
				ap_apicid |= ((node >> 1) & 0x3) << 4;			/* Node ID */
				ap_apicid |= ((node & 0x1) * (siblings + 1)) + core;	/* Core ID */
			} else {
				ap_apicid |= node & 0x3;					/* Node ID */
				ap_apicid |= (((node & 0x1) * (siblings + 1)) + core) << 4;	/* Core ID */
			}
		}
	} else {
		if (fam15h) {
			ap_apicid = 0;
			ap_apicid |= (node & 0x7) << 4;	/* Node ID */
			ap_apicid |= core & 0xf;	/* Core ID */
		} else {
			ap_apicid = node * (nb_cfg_54 ? (siblings + 1) : 1) +
					core * (nb_cfg_54 ? 1 : 64);
		}
	}

	printk(BIOS_DEBUG, "%s: using %d as APIC ID for node %d, core %d\n", __func__, ap_apicid, node, core);

	return ap_apicid;
}

//core_range = 0 : all cores
//core range = 1 : core 0 only
//core range = 2 : cores other than core0

static void for_each_ap(uint32_t bsp_apicid, uint32_t core_range, int8_t node,
			process_ap_t process_ap, void *gp)
{
	// here assume the OS don't change our apicid
	u32 ap_apicid;

	u32 nodes;
	u32 disable_siblings;
	u32 cores_found;
	int i, j;

	/* get_nodes define in ht_wrapper.c */
	nodes = get_nodes();

	if (!IS_ENABLED(CONFIG_LOGICAL_CPUS) ||
	    read_option(multi_core, 0) != 0) {	// 0 means multi core
		disable_siblings = 1;
	} else {
		disable_siblings = 0;
	}

	for (i = 0; i < nodes; i++) {
		if ((node >= 0) && (i != node))
			continue;

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
			ap_apicid = get_boot_apic_id(i, j);

#if IS_ENABLED(CONFIG_ENABLE_APIC_EXT_ID) && (CONFIG_APIC_ID_OFFSET > 0)
#if !IS_ENABLED(CONFIG_LIFT_BSP_APIC_ID)
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

#if IS_ENABLED(CONFIG_SET_FIDVID)
static void init_fidvid_ap(u32 apicid, u32 nodeid, u32 coreid);
#endif

static __always_inline
void print_apicid_nodeid_coreid(u32 apicid, struct node_core_id id,
				const char *str)
{
	printk(BIOS_DEBUG,
	       "%s --- { APICID = %02x NODEID = %02x COREID = %02x} ---\n", str,
	       apicid, id.nodeid, id.coreid);
}

uint32_t wait_cpu_state(uint32_t apicid, uint32_t state, uint32_t state2)
{
	u32 readback = 0;
	u32 timeout = 1;
	int loop = 4000000;
	while (--loop > 0) {
		if (lapic_remote_read(apicid, LAPIC_MSG_REG, &readback) != 0)
			continue;
		if ((readback & 0x3f) == state || (readback & 0x3f) == state2 || (readback & 0x3f) == F10_APSTATE_RESET) {
			timeout = 0;
			break;	//target CPU is in stage started
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
	timeout = wait_cpu_state(ap_apicid, F10_APSTATE_STARTED, F10_APSTATE_ASLEEP);
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
	for_each_ap(bsp_apicid, 2, -1, wait_ap_started, (void *)0);
	printk(BIOS_DEBUG, "\n");
}

void allow_all_aps_stop(u32 bsp_apicid)
{
	/* Called by the BSP to indicate AP can stop */

	/* FIXME Do APs use this? */

	// allow aps to stop use 6 bits for state
	lapic_write(LAPIC_MSG_REG, (bsp_apicid << 24) | F10_APSTATE_STOPPED);
}

static void wait_ap_stopped(u32 ap_apicid, void *gp)
{
	u32 timeout;
	timeout = wait_cpu_state(ap_apicid, F10_APSTATE_ASLEEP, F10_APSTATE_ASLEEP);
	printk(BIOS_DEBUG, "* AP %02x", ap_apicid);
	if (timeout) {
		printk(BIOS_DEBUG, " timed out:%08x\n", timeout);
	} else {
		printk(BIOS_DEBUG, "stopped\n");
	}
}

void wait_all_other_cores_stopped(u32 bsp_apicid)
{
	// all aps other than core0
	printk(BIOS_DEBUG, "stopped ap apicid: ");
	for_each_ap(bsp_apicid, 2, -1, wait_ap_stopped, (void *)0);
	printk(BIOS_DEBUG, "\n");
}

static void enable_apic_ext_id(u32 node)
{
	u32 val;

	val = pci_read_config32(NODE_HT(node), 0x68);
	val |= (HTTC_APIC_EXT_SPUR | HTTC_APIC_EXT_ID | HTTC_APIC_EXT_BRD_CST);
	pci_write_config32(NODE_HT(node), 0x68, val);
}

static void STOP_CAR_AND_CPU(uint8_t skip_sharedc_config, uint32_t apicid)
{
	msr_t msr;
	uint32_t family;

	family = amd_fam1x_cpu_family();		// inline

	if (family < 0x6f) {
		/* Family 10h or earlier */

		/* Disable L2 IC to L3 connection (Only for CAR) */
		msr = rdmsr(BU_CFG2);
		msr.lo &= ~(1 << ClLinesToNbDis);
		wrmsr(BU_CFG2, msr);
	} else {
		/* Family 15h or later
		 * DRAM setup is delayed on Fam15 in order to prevent
		 * any DRAM access before ECC check bits are initialized.
		 * Each core also needs to have its initial DRAM map initialized
		 * before it is put to sleep, otherwise it will fail to wake
		 * in ramstage.  To meet both of these goals, delay DRAM map
		 * setup until the last possible moment, where speculative
		 * memory access is highly unlikely before core halt...
		 */
		if (!skip_sharedc_config) {
			/* Enable memory access for first MBs using top_mem */
			msr.hi = 0;
			msr.lo = (CONFIG_RAMTOP + TOP_MEM_MASK) & (~TOP_MEM_MASK);
			wrmsr(TOP_MEM, msr);
		}
	}

	disable_cache_as_ram_real(skip_sharedc_config);	// inline

	/* Mark the core as sleeping */
	lapic_write(LAPIC_MSG_REG, (apicid << 24) | F10_APSTATE_ASLEEP);

	/* stop all cores except node0/core0 the bsp .... */
	stop_this_cpu();
}

u32 init_cpus(u32 cpu_init_detectedx, struct sys_info *sysinfo)
{
	uint32_t bsp_apicid = 0;
	uint32_t apicid;
	uint32_t dword;
	uint8_t set_mtrrs;
	uint8_t node_count;
	uint8_t fam15_bsp_core1_apicid;
	struct node_core_id id;

	/* Please refer to the calculations and explaination in cache_as_ram.inc before modifying these values */
	uint32_t max_ap_stack_region_size = CONFIG_MAX_CPUS * CONFIG_DCACHE_AP_STACK_SIZE;
	uint32_t max_bsp_stack_region_size = CONFIG_DCACHE_BSP_STACK_SIZE + CONFIG_DCACHE_BSP_STACK_SLUSH;
	uint32_t bsp_stack_region_upper_boundary = CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE;
	uint32_t bsp_stack_region_lower_boundary = bsp_stack_region_upper_boundary - max_bsp_stack_region_size;
	void *lower_stack_region_boundary = (void *)(bsp_stack_region_lower_boundary - max_ap_stack_region_size);
	if (((void*)(sysinfo + 1)) > lower_stack_region_boundary)
		printk(BIOS_WARNING,
			"sysinfo extends into stack region (sysinfo range: [%p,%p] lower stack region boundary: %p)\n",
			sysinfo, sysinfo + 1, lower_stack_region_boundary);

	/*
	 * already set early mtrr in cache_as_ram.inc
	 */

	/* that is from initial apicid, we need nodeid and coreid
	   later */
	id = get_node_core_id_x();

	/* NB_CFG MSR is shared between cores, so we need make sure
	   core0 is done at first --- use wait_all_core0_started  */
	if (id.coreid == 0) {
		/* Set InitApicIdCpuIdLo / EnableCf8ExtCfg on core0 only */
		if (!is_fam15h())
			set_apicid_cpuid_lo();
		set_EnableCf8ExtCfg();
#if IS_ENABLED(CONFIG_ENABLE_APIC_EXT_ID)
		enable_apic_ext_id(id.nodeid);
#endif
	}

	enable_lapic();

#if IS_ENABLED(CONFIG_ENABLE_APIC_EXT_ID) && (CONFIG_APIC_ID_OFFSET > 0)
	u32 initial_apicid = get_initial_apicid();

#if !IS_ENABLED(CONFIG_LIFT_BSP_APIC_ID)
	if (initial_apicid != 0)	// other than bsp
#endif
	{
		/* use initial APIC id to lift it */
		u32 dword = lapic_read(LAPIC_ID);
		dword &= ~(0xff << 24);
		dword |=
		    (((initial_apicid + CONFIG_APIC_ID_OFFSET) & 0xff) << 24);

		lapic_write(LAPIC_ID, dword);
	}
#if IS_ENABLED(CONFIG_LIFT_BSP_APIC_ID)
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
	printk(BIOS_DEBUG, "CPU APICID %02x start flag set\n", apicid);

	if (apicid != bsp_apicid) {
		/* Setup each AP's cores MSRs.
		 * This happens after HTinit.
		 * The BSP runs this code in it's own path.
		 */
		update_microcode(cpuid_eax(1));

		cpuSetAMDMSR(id.nodeid);

		/* Set up HyperTransport probe filter support */
		if (is_gt_rev_d()) {
			dword = pci_read_config32(NODE_PCI(id.nodeid, 0), 0x60);
			node_count = ((dword >> 4) & 0x7) + 1;

			if (node_count > 1) {
				msr_t msr = rdmsr(BU_CFG2_MSR);
				msr.hi |= 1 << (42 - 32);
				wrmsr(BU_CFG2_MSR, msr);
			}
		}

#if IS_ENABLED(CONFIG_SET_FIDVID)
#if IS_ENABLED(CONFIG_LOGICAL_CPUS) && IS_ENABLED(CONFIG_SET_FIDVID_CORE0_ONLY)
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

		if (is_fam15h()) {
			/* core 1 on node 0 is special; to avoid corrupting the
			 * BSP do not alter MTRRs on that core */
			if (IS_ENABLED(CONFIG_ENABLE_APIC_EXT_ID) && (CONFIG_APIC_ID_OFFSET > 0))
				fam15_bsp_core1_apicid = CONFIG_APIC_ID_OFFSET + 1;
			else
				fam15_bsp_core1_apicid = 1;

			if (apicid == fam15_bsp_core1_apicid)
				set_mtrrs = 0;
			else
				set_mtrrs = !!(apicid & 0x1);
		} else {
			set_mtrrs = 1;
		}

		/* AP is ready, configure MTRRs and go to sleep */
		if (set_mtrrs)
			set_var_mtrr(0, 0x00000000, CACHE_TMP_RAMTOP, MTRR_TYPE_WRBACK);

		printk(BIOS_DEBUG, "Disabling CAR on AP %02x\n", apicid);
		if (is_fam15h()) {
			/* Only modify the MSRs on the odd cores (the last cores to finish booting) */
			STOP_CAR_AND_CPU(!set_mtrrs, apicid);
		} else {
			/* Modify MSRs on all cores */
			STOP_CAR_AND_CPU(0, apicid);
		}

		printk(BIOS_DEBUG,
		       "\nAP %02x should be halted but you are reading this....\n",
		       apicid);
	}

	return bsp_apicid;
}

static u32 is_core0_started(u32 nodeid)
{
	u32 htic;
	pci_devfn_t device;
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

#if IS_ENABLED(CONFIG_NORTHBRIDGE_AMD_AMDFAM10)
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

//it is running on core0 of node0
void start_other_cores(uint32_t bsp_apicid)
{
	u32 nodes;
	u32 nodeid;

	// disable multi_core
	if (read_option(multi_core, 0) != 0)  {
		printk(BIOS_DEBUG, "Skip additional core init\n");
		return;
	}

	nodes = get_nodes();

	for (nodeid = 0; nodeid < nodes; nodeid++) {
		u32 cores = get_core_num_in_bsp(nodeid);
		printk(BIOS_DEBUG, "init node: %02x  cores: %02x pass 1\n", nodeid, cores);
		if (cores > 0) {
			real_start_other_core(nodeid, cores);
#ifdef FAM10_AP_NODE_SEQUENTIAL_START
			printk(BIOS_DEBUG, "waiting for core start on node %d...\n", nodeid);
			for_each_ap(bsp_apicid, 2, nodeid, wait_ap_started, (void *)0);
			printk(BIOS_DEBUG, "...started\n");
#endif
		}
	}
}

static void AMD_Errata281(u8 node, uint64_t revision, u32 platform)
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
static BOOL AMD_CpuFindCapability(u8 node, u8 cap_count, u8 *offset)
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
static u32 AMD_checkLinkType(u8 node, u8 regoff)
{
	uint32_t val;
	uint32_t val2;
	uint32_t linktype = 0;

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
		val = (val >> 8) & 0xf;
		if (is_gt_rev_d()) {
			val2 = pci_read_config32(NODE_PCI(node, 0), regoff + 0x1c);
			val |= (val2 & 0x1) << 4;
		}

		if (val > 6)
			linktype |= HTPHY_LINKTYPE_HT3;
		else
			linktype |= HTPHY_LINKTYPE_HT1;

		/* Check ganged */
		val = pci_read_config32(NODE_PCI(node, 0), (((regoff - 0x80) / 0x20) << 2) + 0x170);

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

	/* Determine if link is connected and abort if not */
	if (!(pci_read_config32(NODE_PCI(node, 0), 0x98 + (link * 0x20)) & 0x1))
		return;

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

void cpuSetAMDMSR(uint8_t node_id)
{
	/* This routine loads the CPU with default settings in fam10_msr_default
	 * table . It must be run after Cache-As-RAM has been enabled, and
	 * Hypertransport initialization has taken place.  Also note
	 * that it is run on the current processor only, and only for the current
	 * processor core.
	 */
	msr_t msr;
	u8 i;
	uint8_t nvram;
	u32 platform;
	uint64_t revision;
	uint8_t enable_c_states;
	uint8_t enable_cpb;

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

	/* Revision C0 and above */
	if (revision & AMD_OR_C0) {
		uint8_t enable_experimental_memory_speed_boost;

		/* Check to see if cache partitioning is allowed */
		enable_experimental_memory_speed_boost = 0;
		if (get_option(&nvram, "experimental_memory_speed_boost") == CB_SUCCESS)
			enable_experimental_memory_speed_boost = !!nvram;

		uint32_t f3x1fc = pci_read_config32(NODE_PCI(node_id, 3), 0x1fc);
		msr = rdmsr(FP_CFG);
		msr.hi &= ~(0x7 << (42-32));			/* DiDtCfg4 */
		msr.hi |= (((f3x1fc >> 17) & 0x7) << (42-32));
		msr.hi &= ~(0x1 << (41-32));			/* DiDtCfg5 */
		msr.hi |= (((f3x1fc >> 22) & 0x1) << (41-32));
		msr.hi &= ~(0x1 << (40-32));			/* DiDtCfg3 */
		msr.hi |= (((f3x1fc >> 16) & 0x1) << (40-32));
		msr.hi &= ~(0x7 << (32-32));			/* DiDtCfg1 (1) */
		msr.hi |= (((f3x1fc >> 11) & 0x7) << (32-32));
		msr.lo &= ~(0x1f << 27);			/* DiDtCfg1 (2) */
		msr.lo |= (((f3x1fc >> 6) & 0x1f) << 27);
		msr.lo &= ~(0x3 << 25);				/* DiDtCfg2 */
		msr.lo |= (((f3x1fc >> 14) & 0x3) << 25);
		msr.lo &= ~(0x1f << 18);			/* DiDtCfg0 */
		msr.lo |= (((f3x1fc >> 1) & 0x1f) << 18);
		msr.lo &= ~(0x1 << 16);				/* DiDtMode */
		msr.lo |= ((f3x1fc & 0x1) << 16);
		wrmsr(FP_CFG, msr);

		if (enable_experimental_memory_speed_boost) {
			msr = rdmsr(BU_CFG3);
			msr.lo |= (0x3 << 20);			/* PfcStrideMul = 0x3 */
			wrmsr(BU_CFG3, msr);
		}
	}

#if IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_SB700) || IS_ENABLED(CONFIG_SOUTHBRIDGE_AMD_SB800)
	if (revision & (AMD_DR_GT_D0 | AMD_FAM15_ALL)) {
		/* Set up message triggered C1E */
		msr = rdmsr(0xc0010055);
		msr.lo &= ~0xffff;		/* IOMsgAddr = ACPI_PM_EVT_BLK */
		msr.lo |= ACPI_PM_EVT_BLK & 0xffff;
		msr.lo |= (0x1 << 29);		/* BmStsClrOnHltEn = 1 */
		if (revision & AMD_DR_GT_D0) {
			msr.lo &= ~(0x1 << 28);	/* C1eOnCmpHalt = 0 */
			msr.lo &= ~(0x1 << 27);	/* SmiOnCmpHalt = 0 */
		}
		wrmsr(0xc0010055, msr);

		msr = rdmsr(0xc0010015);
		msr.lo |= (0x1 << 12);		/* HltXSpCycEn = 1 */
		wrmsr(0xc0010015, msr);
	}

	if (revision & (AMD_DR_Ex | AMD_FAM15_ALL)) {
		enable_c_states = 0;
		if (IS_ENABLED(CONFIG_HAVE_ACPI_TABLES))
			if (get_option(&nvram, "cpu_c_states") == CB_SUCCESS)
				enable_c_states = !!nvram;

		if (enable_c_states) {
			/* Set up the C-state base address */
			msr_t c_state_addr_msr;
			c_state_addr_msr = rdmsr(0xc0010073);
			c_state_addr_msr.lo = ACPI_CPU_P_LVL2;	/* CstateAddr = ACPI_CPU_P_LVL2 */
			wrmsr(0xc0010073, c_state_addr_msr);
		}
	}
#else
	enable_c_states = 0;
#endif

	if (revision & AMD_FAM15_ALL) {
		enable_cpb = 1;
		if (get_option(&nvram, "cpu_core_boost") == CB_SUCCESS)
			enable_cpb = !!nvram;

		if (!enable_cpb) {
			/* Disable Core Performance Boost */
			msr = rdmsr(0xc0010015);
			msr.lo |= (0x1 << 25);		/* CpbDis = 1 */
			wrmsr(0xc0010015, msr);
		}
	}

	printk(BIOS_DEBUG, " done\n");
}

static void cpuSetAMDPCI(u8 node)
{
	/* This routine loads the CPU with default settings in fam10_pci_default
	 * table . It must be run after Cache-As-RAM has been enabled, and
	 * Hypertransport initialization has taken place.  Also note
	 * that it is run for the first core on each node
	 */
	uint8_t i;
	uint8_t j;
	u32 platform;
	u32 val;
	uint8_t offset;
	uint32_t dword;
	uint64_t revision;

	/* FIXME
	 * This should be configurable
	 */
	uint8_t sockets = 2;
	uint8_t sockets_populated = 2;

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

	if (is_fam15h()) {
		if (CONFIG_CPU_SOCKET_TYPE == 0x14) {
			/* Socket C32 */
			dword = pci_read_config32(NODE_PCI(node, 0), 0x84);
			dword |= 0x1 << 13;			/* LdtStopTriEn = 1 */
			pci_write_config32(NODE_PCI(node, 0), 0x84, dword);

			dword = pci_read_config32(NODE_PCI(node, 0), 0xa4);
			dword |= 0x1 << 13;			/* LdtStopTriEn = 1 */
			pci_write_config32(NODE_PCI(node, 0), 0xa4, dword);

			dword = pci_read_config32(NODE_PCI(node, 0), 0xc4);
			dword |= 0x1 << 13;			/* LdtStopTriEn = 1 */
			pci_write_config32(NODE_PCI(node, 0), 0xc4, dword);

			dword = pci_read_config32(NODE_PCI(node, 0), 0xe4);
			dword |= 0x1 << 13;			/* LdtStopTriEn = 1 */
			pci_write_config32(NODE_PCI(node, 0), 0xe4, dword);
		}
		else {
			/* Other socket (G34, etc.) */
			dword = pci_read_config32(NODE_PCI(node, 0), 0x84);
			dword &= ~(0x1 << 13);			/* LdtStopTriEn = 0 */
			pci_write_config32(NODE_PCI(node, 0), 0x84, dword);

			dword = pci_read_config32(NODE_PCI(node, 0), 0xa4);
			dword &= ~(0x1 << 13);			/* LdtStopTriEn = 0 */
			pci_write_config32(NODE_PCI(node, 0), 0xa4, dword);

			dword = pci_read_config32(NODE_PCI(node, 0), 0xc4);
			dword &= ~(0x1 << 13);			/* LdtStopTriEn = 0 */
			pci_write_config32(NODE_PCI(node, 0), 0xc4, dword);

			dword = pci_read_config32(NODE_PCI(node, 0), 0xe4);
			dword &= ~(0x1 << 13);			/* LdtStopTriEn = 0 */
			pci_write_config32(NODE_PCI(node, 0), 0xe4, dword);
		}
	}

#ifdef DEBUG_HT_SETUP
	/* Dump link settings */
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			printk(BIOS_DEBUG, "Node %d link %d: type register: %08x control register: %08x extended control sublink 0: %08x 1: %08x\n", i, j,
				pci_read_config32(NODE_PCI(i, 0), 0x98 + (j * 0x20)), pci_read_config32(NODE_PCI(i, 0), 0x84 + (j * 0x20)),
				pci_read_config32(NODE_PCI(i, 0), 0x170 + (j * 0x4)), pci_read_config32(NODE_PCI(i, 0), 0x180 + (j * 0x4)));
		}
	}
#endif

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
					if (AMD_checkLinkType(node, offset)
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

	if (revision & (AMD_DR_GT_D0 | AMD_FAM15_ALL)) {
		/* Set up message triggered C1E */
		dword = pci_read_config32(NODE_PCI(node, 3), 0xd4);
		dword &= ~(0x1 << 14);			/* CacheFlushImmOnAllHalt = !is_fam15h() */
		dword |= (is_fam15h()?0:1) << 14;
		pci_write_config32(NODE_PCI(node, 3), 0xd4, dword);

		dword = pci_read_config32(NODE_PCI(node, 3), 0xdc);
		dword |= 0x1 << 26;			/* IgnCpuPrbEn = 1 */
		dword &= ~(0x7f << 19);			/* CacheFlushOnHaltTmr = 0x28 */
		dword |= 0x28 << 19;
		dword |= 0x7 << 16;			/* CacheFlushOnHaltCtl = 0x7 */
		pci_write_config32(NODE_PCI(node, 3), 0xdc, dword);

		dword = pci_read_config32(NODE_PCI(node, 3), 0xa0);
		dword |= 0x1 << 10;			/* IdleExitEn = 1 */
		pci_write_config32(NODE_PCI(node, 3), 0xa0, dword);

		if (revision & AMD_DR_GT_D0) {
			dword = pci_read_config32(NODE_PCI(node, 3), 0x188);
			dword |= 0x1 << 4;			/* EnStpGntOnFlushMaskWakeup = 1 */
			pci_write_config32(NODE_PCI(node, 3), 0x188, dword);
		} else {
			dword = pci_read_config32(NODE_PCI(node, 4), 0x128);
			dword &= ~(0x1 << 31);			/* CstateMsgDis = 0 */
			pci_write_config32(NODE_PCI(node, 4), 0x128, dword);
		}

		dword = pci_read_config32(NODE_PCI(node, 3), 0xd4);
		dword |= 0x1 << 13;			/* MTC1eEn = 1 */
		pci_write_config32(NODE_PCI(node, 3), 0xd4, dword);
	}

	if (revision & AMD_FAM15_ALL) {
		uint32_t f5x80;
		uint8_t cu_enabled;
		uint8_t compute_unit_count = 0;
		uint8_t compute_unit_buffer_count;

		uint32_t f3xe8;
		uint8_t dual_node = 0;

		f3xe8 = pci_read_config32(NODE_PCI(0, 3), 0xe8);

		/* Check for dual node capability */
		if (f3xe8 & 0x20000000)
			dual_node = 1;

		/* Determine the number of active compute units on this node */
		f5x80 = pci_read_config32(NODE_PCI(node, 5), 0x80);
		cu_enabled = f5x80 & 0xf;
		if (cu_enabled == 0x1)
			compute_unit_count = 1;
		if (cu_enabled == 0x3)
			compute_unit_count = 2;
		if (cu_enabled == 0x7)
			compute_unit_count = 3;
		if (cu_enabled == 0xf)
			compute_unit_count = 4;

		if (compute_unit_count == 1)
			compute_unit_buffer_count = 0x1c;
		else if (compute_unit_count == 2)
			compute_unit_buffer_count = 0x18;
		else if (compute_unit_count == 3)
			compute_unit_buffer_count = 0x14;
		else
			compute_unit_buffer_count = 0x10;

		dword = pci_read_config32(NODE_PCI(node, 3), 0x1a0);
		dword &= ~(0x1f << 4);			/* L3FreeListCBC = compute_unit_buffer_count */
		dword |= (compute_unit_buffer_count << 4);
		pci_write_config32(NODE_PCI(node, 3), 0x1a0, dword);

		uint8_t link;
		uint8_t link_real;
		uint8_t ganged;
		uint8_t iolink;
		uint8_t probe_filter_enabled = !!dual_node;

		/* Set up the Link Base Channel Buffer Count */
		uint8_t isoc_rsp_data;
		uint8_t isoc_np_req_data;
		uint8_t isoc_rsp_cmd;
		uint8_t isoc_preq;
		uint8_t isoc_np_req_cmd;
		uint8_t free_data;
		uint8_t free_cmd;
		uint8_t rsp_data;
		uint8_t np_req_data;
		uint8_t probe_cmd;
		uint8_t rsp_cmd;
		uint8_t preq;
		uint8_t np_req_cmd;

		/* Common settings for all links and system configurations */
		isoc_rsp_data = 0;
		isoc_np_req_data = 0;
		isoc_rsp_cmd = 0;
		isoc_preq = 0;
		isoc_np_req_cmd = 1;
		free_cmd = 8;

		for (link = 0; link < 4; link++) {
			if (AMD_CpuFindCapability(node, link, &offset)) {
				link_real = (offset - 0x80) / 0x20;
				ganged = !!(pci_read_config32(NODE_PCI(node, 0), (link_real << 2) + 0x170) & 0x1);
				iolink = !!(AMD_checkLinkType(node, offset) & HTPHY_LINKTYPE_NONCOHERENT);

				if (!iolink && ganged) {
					if (probe_filter_enabled) {
						free_data = 0;
						rsp_data = 3;
						np_req_data = 3;
						probe_cmd = 4;
						rsp_cmd = 9;
						preq = 2;
						np_req_cmd = 8;
					} else {
						free_data = 0;
						rsp_data = 3;
						np_req_data = 3;
						probe_cmd = 8;
						rsp_cmd = 9;
						preq = 2;
						np_req_cmd = 4;
					}
				} else if (!iolink && !ganged) {
					if (probe_filter_enabled) {
						free_data = 0;
						rsp_data = 3;
						np_req_data = 3;
						probe_cmd = 4;
						rsp_cmd = 9;
						preq = 2;
						np_req_cmd = 8;
					} else {
						free_data = 0;
						rsp_data = 3;
						np_req_data = 3;
						probe_cmd = 8;
						rsp_cmd = 9;
						preq = 2;
						np_req_cmd = 4;
					}
				} else if (iolink && ganged) {
					free_data = 0;
					rsp_data = 1;
					np_req_data = 0;
					probe_cmd = 0;
					rsp_cmd = 2;
					preq = 7;
					np_req_cmd = 14;
				} else {
					/* FIXME
					 * This is an educated guess as the BKDG does not specify
					 * the appropriate buffer counts for this case!
					 */
					free_data = 1;
					rsp_data = 1;
					np_req_data = 1;
					probe_cmd = 0;
					rsp_cmd = 2;
					preq = 4;
					np_req_cmd = 12;
				}

				dword = pci_read_config32(NODE_PCI(node, 0), (link_real * 0x20) + 0x94);
				dword &= ~(0x3 << 27);			/* IsocRspData = isoc_rsp_data */
				dword |= ((isoc_rsp_data & 0x3) << 27);
				dword &= ~(0x3 << 25);			/* IsocNpReqData = isoc_np_req_data */
				dword |= ((isoc_np_req_data & 0x3) << 25);
				dword &= ~(0x7 << 22);			/* IsocRspCmd = isoc_rsp_cmd */
				dword |= ((isoc_rsp_cmd & 0x7) << 22);
				dword &= ~(0x7 << 19);			/* IsocPReq = isoc_preq */
				dword |= ((isoc_preq & 0x7) << 19);
				dword &= ~(0x7 << 16);			/* IsocNpReqCmd = isoc_np_req_cmd */
				dword |= ((isoc_np_req_cmd & 0x7) << 16);
				pci_write_config32(NODE_PCI(node, 0), (link_real * 0x20) + 0x94, dword);

				dword = pci_read_config32(NODE_PCI(node, 0), (link_real * 0x20) + 0x90);
				dword &= ~(0x1 << 31);			/* LockBc = 0x1 */
				dword |= ((0x1 & 0x1) << 31);
				dword &= ~(0x7 << 25);			/* FreeData = free_data */
				dword |= ((free_data & 0x7) << 25);
				dword &= ~(0x1f << 20);			/* FreeCmd = free_cmd */
				dword |= ((free_cmd & 0x1f) << 20);
				dword &= ~(0x3 << 18);			/* RspData = rsp_data */
				dword |= ((rsp_data & 0x3) << 18);
				dword &= ~(0x3 << 16);			/* NpReqData = np_req_data */
				dword |= ((np_req_data & 0x3) << 16);
				dword &= ~(0xf << 12);			/* ProbeCmd = probe_cmd */
				dword |= ((probe_cmd & 0xf) << 12);
				dword &= ~(0xf << 8);			/* RspCmd = rsp_cmd */
				dword |= ((rsp_cmd & 0xf) << 8);
				dword &= ~(0x7 << 5);			/* PReq = preq */
				dword |= ((preq & 0x7) << 5);
				dword &= ~(0x1f << 0);			/* NpReqCmd = np_req_cmd */
				dword |= ((np_req_cmd & 0x1f) << 0);
				pci_write_config32(NODE_PCI(node, 0), (link_real * 0x20) + 0x90, dword);
			}
		}

		/* Set up the Link to XCS Token Counts */
		uint8_t isoc_rsp_tok_1;
		uint8_t isoc_preq_tok_1;
		uint8_t isoc_req_tok_1;
		uint8_t probe_tok_1;
		uint8_t rsp_tok_1;
		uint8_t preq_tok_1;
		uint8_t req_tok_1;
		uint8_t isoc_rsp_tok_0;
		uint8_t isoc_preq_tok_0;
		uint8_t isoc_req_tok_0;
		uint8_t free_tokens;
		uint8_t probe_tok_0;
		uint8_t rsp_tok_0;
		uint8_t preq_tok_0;
		uint8_t req_tok_0;

		for (link = 0; link < 4; link++) {
			if (AMD_CpuFindCapability(node, link, &offset)) {
				link_real = (offset - 0x80) / 0x20;
				ganged = !!(pci_read_config32(NODE_PCI(node, 0), (link_real << 2) + 0x170) & 0x1);
				iolink = !!(AMD_checkLinkType(node, offset) & HTPHY_LINKTYPE_NONCOHERENT);

				/* Set defaults */
				isoc_rsp_tok_1 = 0;
				isoc_preq_tok_1 = 0;
				isoc_req_tok_1 = 0;
				probe_tok_1 = !ganged;
				rsp_tok_1 = !ganged;
				preq_tok_1 = !ganged;
				req_tok_1 = !ganged;
				isoc_rsp_tok_0 = 0;
				isoc_preq_tok_0 = 0;
				isoc_req_tok_0 = 0;
				free_tokens = 0;
				probe_tok_0 = ((ganged)?2:1);
				rsp_tok_0 = ((ganged)?2:1);
				preq_tok_0 = ((ganged)?2:1);
				req_tok_0 = ((ganged)?2:1);

				if (!iolink && ganged) {
					if (!dual_node) {
						isoc_rsp_tok_1 = 0;
						isoc_preq_tok_1 = 0;
						isoc_req_tok_1 = 0;
						probe_tok_1 = 0;
						rsp_tok_1 = 0;
						preq_tok_1 = 0;
						req_tok_1 = 0;
						isoc_rsp_tok_0 = 0;
						isoc_preq_tok_0 = 0;
						isoc_req_tok_0 = 1;
						free_tokens = 3;
						probe_tok_0 = 2;
						rsp_tok_0 = 2;
						preq_tok_0 = 2;
						req_tok_0 = 2;
					} else {
						if ((sockets == 1)
							|| ((sockets == 2) && (sockets_populated == 1))) {
							isoc_rsp_tok_1 = 0;
							isoc_preq_tok_1 = 0;
							isoc_req_tok_1 = 0;
							probe_tok_1 = 0;
							rsp_tok_1 = 0;
							preq_tok_1 = 0;
							req_tok_1 = 0;
							isoc_rsp_tok_0 = 0;
							isoc_preq_tok_0 = 0;
							isoc_req_tok_0 = 1;
							free_tokens = 0;
							probe_tok_0 = 2;
							rsp_tok_0 = 2;
							preq_tok_0 = 2;
							req_tok_0 = 2;
						} else if (((sockets == 2) && (sockets_populated == 2))
							|| ((sockets == 4) && (sockets_populated == 2))) {
							isoc_rsp_tok_1 = 0;
							isoc_preq_tok_1 = 0;
							isoc_req_tok_1 = 0;
							probe_tok_1 = 0;
							rsp_tok_1 = 0;
							preq_tok_1 = 0;
							req_tok_1 = 0;
							isoc_rsp_tok_0 = 0;
							isoc_preq_tok_0 = 0;
							isoc_req_tok_0 = 1;
							free_tokens = 0;
							probe_tok_0 = 1;
							rsp_tok_0 = 2;
							preq_tok_0 = 2;
							req_tok_0 = 2;
						} else if ((sockets == 4) && (sockets_populated == 4)) {
							isoc_rsp_tok_1 = 0;
							isoc_preq_tok_1 = 0;
							isoc_req_tok_1 = 0;
							probe_tok_1 = 0;
							rsp_tok_1 = 0;
							preq_tok_1 = 0;
							req_tok_1 = 0;
							isoc_rsp_tok_0 = 0;
							isoc_preq_tok_0 = 0;
							isoc_req_tok_0 = 1;
							free_tokens = 0;
							probe_tok_0 = 2;
							rsp_tok_0 = 1;
							preq_tok_0 = 1;
							req_tok_0 = 2;
						}
					}
				} else if (!iolink && !ganged) {
					if ((sockets == 1)
						|| ((sockets == 2) && (sockets_populated == 1))) {
						if (probe_filter_enabled) {
							isoc_rsp_tok_1 = 0;
							isoc_preq_tok_1 = 0;
							isoc_req_tok_1 = 0;
							probe_tok_1 = 1;
							rsp_tok_1 = 1;
							preq_tok_1 = 1;
							req_tok_1 = 1;
							isoc_rsp_tok_0 = 0;
							isoc_preq_tok_0 = 0;
							isoc_req_tok_0 = 1;
							free_tokens = 0;
							probe_tok_0 = 1;
							rsp_tok_0 = 2;
							preq_tok_0 = 1;
							req_tok_0 = 1;
						} else {
							isoc_rsp_tok_1 = 0;
							isoc_preq_tok_1 = 0;
							isoc_req_tok_1 = 0;
							probe_tok_1 = 1;
							rsp_tok_1 = 1;
							preq_tok_1 = 1;
							req_tok_1 = 1;
							isoc_rsp_tok_0 = 0;
							isoc_preq_tok_0 = 0;
							isoc_req_tok_0 = 1;
							free_tokens = 0;
							probe_tok_0 = 1;
							rsp_tok_0 = 1;
							preq_tok_0 = 1;
							req_tok_0 = 1;
						}
					} else if ((sockets == 2) && (sockets_populated == 2)) {
						isoc_rsp_tok_1 = 0;
						isoc_preq_tok_1 = 0;
						isoc_req_tok_1 = 1;
						probe_tok_1 = 1;
						rsp_tok_1 = 1;
						preq_tok_1 = 1;
						req_tok_1 = 1;
						isoc_rsp_tok_0 = 0;
						isoc_preq_tok_0 = 0;
						isoc_req_tok_0 = 1;
						free_tokens = 2;
						probe_tok_0 = 1;
						rsp_tok_0 = 1;
						preq_tok_0 = 1;
						req_tok_0 = 1;
					} else if ((sockets == 4) && (sockets_populated == 2)) {
						isoc_rsp_tok_1 = 0;
						isoc_preq_tok_1 = 0;
						isoc_req_tok_1 = 1;
						probe_tok_1 = 1;
						rsp_tok_1 = 1;
						preq_tok_1 = 1;
						req_tok_1 = 1;
						isoc_rsp_tok_0 = 0;
						isoc_preq_tok_0 = 0;
						isoc_req_tok_0 = 1;
						free_tokens = 4;
						probe_tok_0 = 1;
						rsp_tok_0 = 1;
						preq_tok_0 = 1;
						req_tok_0 = 1;
					} else if ((sockets == 4) && (sockets_populated == 4)) {
						isoc_rsp_tok_1 = 0;
						isoc_preq_tok_1 = 0;
						isoc_req_tok_1 = 1;
						probe_tok_1 = 1;
						rsp_tok_1 = 1;
						preq_tok_1 = 1;
						req_tok_1 = 1;
						isoc_rsp_tok_0 = 0;
						isoc_preq_tok_0 = 0;
						isoc_req_tok_0 = 1;
						free_tokens = 0;
						probe_tok_0 = 1;
						rsp_tok_0 = 1;
						preq_tok_0 = 1;
						req_tok_0 = 1;
					}
				} else if (iolink && ganged) {
					if (!dual_node) {
						isoc_rsp_tok_1 = 0;
						isoc_preq_tok_1 = 0;
						isoc_req_tok_1 = 0;
						probe_tok_1 = 0;
						rsp_tok_1 = 0;
						preq_tok_1 = 0;
						req_tok_1 = 0;
						isoc_rsp_tok_0 = 0;
						isoc_preq_tok_0 = 0;
						isoc_req_tok_0 = 1;
						free_tokens = 3;
						probe_tok_0 = 0;
						rsp_tok_0 = 2;
						preq_tok_0 = 2;
						req_tok_0 = 2;
					} else if ((sockets == 1)
						|| (sockets == 2)
						|| ((sockets == 4) && (sockets_populated == 2))) {
						isoc_rsp_tok_1 = 0;
						isoc_preq_tok_1 = 0;
						isoc_req_tok_1 = 0;
						probe_tok_1 = 0;
						rsp_tok_1 = 0;
						preq_tok_1 = 0;
						req_tok_1 = 0;
						isoc_rsp_tok_0 = 0;
						isoc_preq_tok_0 = 0;
						isoc_req_tok_0 = 1;
						free_tokens = 0;
						probe_tok_0 = 0;
						rsp_tok_0 = 2;
						preq_tok_0 = 2;
						req_tok_0 = 2;
					} else if ((sockets == 4) && (sockets_populated == 4)) {
						isoc_rsp_tok_1 = 0;
						isoc_preq_tok_1 = 0;
						isoc_req_tok_1 = 0;
						probe_tok_1 = 0;
						rsp_tok_1 = 0;
						preq_tok_1 = 0;
						req_tok_1 = 0;
						isoc_rsp_tok_0 = 0;
						isoc_preq_tok_0 = 0;
						isoc_req_tok_0 = 2;
						free_tokens = 0;
						probe_tok_0 = 2;
						rsp_tok_0 = 2;
						preq_tok_0 = 2;
						req_tok_0 = 2;
					}
				}

				dword = pci_read_config32(NODE_PCI(node, 3), (link_real << 2) + 0x148);
				dword &= ~(0x3 << 30);			/* FreeTok[3:2] = free_tokens[3:2] */
				dword |= (((free_tokens >> 2) & 0x3) << 30);
				dword &= ~(0x1 << 28);			/* IsocRspTok1 = isoc_rsp_tok_1 */
				dword |= (((isoc_rsp_tok_1) & 0x1) << 28);
				dword &= ~(0x1 << 26);			/* IsocPreqTok1 = isoc_preq_tok_1 */
				dword |= (((isoc_preq_tok_1) & 0x1) << 26);
				dword &= ~(0x1 << 24);			/* IsocReqTok1 = isoc_req_tok_1 */
				dword |= (((isoc_req_tok_1) & 0x1) << 24);
				dword &= ~(0x3 << 22);			/* ProbeTok1 = probe_tok_1 */
				dword |= (((probe_tok_1) & 0x3) << 22);
				dword &= ~(0x3 << 20);			/* RspTok1 = rsp_tok_1 */
				dword |= (((rsp_tok_1) & 0x3) << 20);
				dword &= ~(0x3 << 18);			/* PReqTok1 = preq_tok_1 */
				dword |= (((preq_tok_1) & 0x3) << 18);
				dword &= ~(0x3 << 16);			/* ReqTok1 = req_tok_1 */
				dword |= (((req_tok_1) & 0x3) << 16);
				dword &= ~(0x3 << 14);			/* FreeTok[1:0] = free_tokens[1:0] */
				dword |= (((free_tokens) & 0x3) << 14);
				dword &= ~(0x3 << 12);			/* IsocRspTok0 = isoc_rsp_tok_0 */
				dword |= (((isoc_rsp_tok_0) & 0x3) << 12);
				dword &= ~(0x3 << 10);			/* IsocPreqTok0 = isoc_preq_tok_0 */
				dword |= (((isoc_preq_tok_0) & 0x3) << 10);
				dword &= ~(0x3 << 8);			/* IsocReqTok0 = isoc_req_tok_0 */
				dword |= (((isoc_req_tok_0) & 0x3) << 8);
				dword &= ~(0x3 << 6);			/* ProbeTok0 = probe_tok_0 */
				dword |= (((probe_tok_0) & 0x3) << 6);
				dword &= ~(0x3 << 4);			/* RspTok0 = rsp_tok_0 */
				dword |= (((rsp_tok_0) & 0x3) << 4);
				dword &= ~(0x3 << 2);			/* PReqTok0 = preq_tok_0 */
				dword |= (((preq_tok_0) & 0x3) << 2);
				dword &= ~(0x3 << 0);			/* ReqTok0 = req_tok_0 */
				dword |= (((req_tok_0) & 0x3) << 0);
				pci_write_config32(NODE_PCI(node, 3), (link_real << 2) + 0x148, dword);
			}
		}

		/* Set up the SRI to XCS Token Count */
		uint8_t free_tok;
		uint8_t up_rsp_tok;

		/* Set defaults */
		free_tok = 0xa;
		up_rsp_tok = 0x3;

		if (!dual_node) {
			free_tok = 0xa;
			up_rsp_tok = 0x3;
		} else {
			if ((sockets == 1)
				|| ((sockets == 2) && (sockets_populated == 1))) {
				if (probe_filter_enabled) {
					free_tok = 0x9;
					up_rsp_tok = 0x3;
				} else {
					free_tok = 0xa;
					up_rsp_tok = 0x3;
				}
			} else if ((sockets == 2) && (sockets_populated == 2)) {
				free_tok = 0xb;
				up_rsp_tok = 0x1;
			} else if ((sockets == 4) && (sockets_populated == 2)) {
				free_tok = 0xa;
				up_rsp_tok = 0x3;
			} else if ((sockets == 4) && (sockets_populated == 4)) {
				free_tok = 0x9;
				up_rsp_tok = 0x1;
			}
		}

		dword = pci_read_config32(NODE_PCI(node, 3), 0x140);
		dword &= ~(0xf << 20);			/* FreeTok = free_tok */
		dword |= ((free_tok & 0xf) << 20);
		dword &= ~(0x3 << 8);			/* UpRspTok = up_rsp_tok */
		dword |= ((up_rsp_tok & 0x3) << 8);
		pci_write_config32(NODE_PCI(node, 3), 0x140, dword);
	}

	uint8_t link;
	uint8_t link_real;
	uint8_t isochronous;
	uint8_t isochronous_link_present;

	/* Set up isochronous buffers if needed */
	isochronous_link_present = 0;
	if (revision & AMD_FAM15_ALL) {
		for (link = 0; link < 4; link++) {
			if (AMD_CpuFindCapability(node, link, &offset)) {
				link_real = (offset - 0x80) / 0x20;
				isochronous = (pci_read_config32(NODE_PCI(node, 0), (link_real * 0x20) + 0x84) >> 12) & 0x1;

				if (isochronous)
					isochronous_link_present = 1;
			}
		}
	}

	uint8_t free_tok;
	uint8_t up_rsp_cbc;
	uint8_t isoc_preq_cbc;
	uint8_t isoc_preq_tok;
	uint8_t xbar_to_sri_free_list_cbc;
	if (isochronous_link_present) {
		/* Adjust buffer counts */
		dword = pci_read_config32(NODE_PCI(node, 3), 0x70);
		isoc_preq_cbc = (dword >> 24) & 0x7;
		up_rsp_cbc = (dword >> 16) & 0x7;
		up_rsp_cbc--;
		isoc_preq_cbc++;
		dword &= ~(0x7 << 24);			/* IsocPreqCBC = isoc_preq_cbc */
		dword |= ((isoc_preq_cbc & 0x7) << 24);
		dword &= ~(0x7 << 16);			/* UpRspCBC = up_rsp_cbc */
		dword |= ((up_rsp_cbc & 0x7) << 16);
		pci_write_config32(NODE_PCI(node, 3), 0x70, dword);

		dword = pci_read_config32(NODE_PCI(node, 3), 0x74);
		isoc_preq_cbc = (dword >> 24) & 0x7;
		isoc_preq_cbc++;
		dword &= ~(0x7 << 24);			/* IsocPreqCBC = isoc_preq_cbc */
		dword |= (isoc_preq_cbc & 0x7) << 24;
		pci_write_config32(NODE_PCI(node, 3), 0x74, dword);

		dword = pci_read_config32(NODE_PCI(node, 3), 0x7c);
		xbar_to_sri_free_list_cbc = dword & 0x1f;
		xbar_to_sri_free_list_cbc--;
		dword &= ~0x1f;				/* Xbar2SriFreeListCBC = xbar_to_sri_free_list_cbc */
		dword |= xbar_to_sri_free_list_cbc & 0x1f;
		pci_write_config32(NODE_PCI(node, 3), 0x7c, dword);

		dword = pci_read_config32(NODE_PCI(node, 3), 0x140);
		free_tok = (dword >> 20) & 0xf;
		isoc_preq_tok = (dword >> 14) & 0x3;
		free_tok--;
		isoc_preq_tok++;
		dword &= ~(0xf << 20);			/* FreeTok = free_tok */
		dword |= ((free_tok & 0xf) << 20);
		dword &= ~(0x3 << 14);			/* IsocPreqTok = isoc_preq_tok */
		dword |= ((isoc_preq_tok & 0x3) << 14);
		pci_write_config32(NODE_PCI(node, 3), 0x140, dword);
	}

	printk(BIOS_DEBUG, " done\n");
}

#ifdef UNUSED_CODE
/* Clearing the MCA registers is apparently handled in the ramstage CPU Function 3 driver */
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
void finalize_node_setup(struct sys_info *sysinfo)
{
	u8 i;
	u8 nodes = get_nodes();
	u32 reg;

	/* read Node0 F0_0x64 bit [8:10] to find out SbLink # */
	reg = pci_read_config32(NODE_HT(0), 0x64);
	sysinfo->sblk = (reg >> 8) & 7;
	sysinfo->sbbusn = 0;
	sysinfo->nodes = nodes;
	sysinfo->sbdn = get_sbdn(sysinfo->sbbusn);

	for (i = 0; i < nodes; i++) {
		cpuSetAMDPCI(i);
	}

#if IS_ENABLED(CONFIG_SET_FIDVID)
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

#if IS_ENABLED(CONFIG_SET_FIDVID)
# include "fidvid.c"
#endif
