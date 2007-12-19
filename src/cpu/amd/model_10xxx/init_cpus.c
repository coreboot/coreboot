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


//it takes the ENABLE_APIC_EXT_ID and APIC_ID_OFFSET and LIFT_BSP_APIC_ID
#ifndef FAM10_SET_FIDVID
	#define FAM10_SET_FIDVID 1
#endif

#ifndef FAM10_SET_FIDVID_CORE0_ONLY
	/* MSR FIDVID_CTL and FIDVID_STATUS are shared by cores,
	   Need to do every AP to set common FID/VID*/
	#define FAM10_SET_FIDVID_CORE0_ONLY 0
#endif


static inline void print_initcpu8 (const char *strval, u8 val)
{
	printk_debug("%s%02x\n", strval, val);
}

static inline void print_initcpu8_nocr (const char *strval, u8 val)
{
	printk_debug("%s%02x", strval, val);
}


static inline void print_initcpu16 (const char *strval, u16 val)
{
	printk_debug("%s%04x\n", strval, val);
}


static inline void print_initcpu(const char *strval, u32 val)
{
	printk_debug("%s%08x\n", strval, val);
}


static void prep_fid_change(void);
static void init_fidvid_stage2(u32 apicid, u32 nodeid);

#if PCI_IO_CFG_EXT == 1
static inline void set_EnableCf8ExtCfg(void)
{
	// set the NB_CFG[46]=1;
	msr_t msr;
	msr = rdmsr(NB_CFG_MSR);
	// EnableCf8ExtCfg: We need that to access PCI_IO_CFG_EXT 4K range
	msr.hi |= (1<<(46-32));
	wrmsr(NB_CFG_MSR, msr);
}
#else
static inline void set_EnableCf8ExtCfg(void) { }
#endif


/*[39:8] */
#define PCI_MMIO_BASE 0xfe000000
/* because we will use gs to store hi, so need to make sure lo can start
   from 0, So PCI_MMIO_BASE & 0x00ffffff should be equal to 0*/

static inline void set_pci_mmio_conf_reg(void)
{
#if MMCONF_SUPPORT
	msr_t msr;
	msr = rdmsr(0xc0010058);
	msr.lo &= ~(0xfff00000 | (0xf << 2));
	// 256 bus per segment, MMIO reg will be 4G , enable MMIO Config space
	msr.lo |= ((8+PCI_BUS_SEGN_BITS) << 2) | (1 << 0);
	msr.hi &= ~(0x0000ffff);
	msr.hi |= (PCI_MMIO_BASE >> (32-8));
	wrmsr(0xc0010058, msr); // MMIO Config Base Address Reg

	//mtrr for that range?
//	set_var_mtrr_x(7, PCI_MMIO_BASE<<8, PCI_MMIO_BASE>>(32-8), 0x00000000, 0x01, MTRR_TYPE_UNCACHEABLE);

	set_wrap32dis();

	msr.hi = (PCI_MMIO_BASE >> (32-8));
	msr.lo = 0;
	wrmsr(0xc0000101, msr); //GS_Base Reg



#endif
}


typedef void (*process_ap_t)(u32 apicid, void *gp);

//core_range = 0 : all cores
//core range = 1 : core 0 only
//core range = 2 : cores other than core0

static void for_each_ap(u32 bsp_apicid, u32 core_range,
				process_ap_t process_ap, void *gp)
{
	// here assume the OS don't change our apicid
	u32 ap_apicid;

	u32 nodes;
	u32 siblings;
	u32 disable_siblings;
	u32 cores_found;
	u32 nb_cfg_54;
	int i,j;
	u32 ApicIdCoreIdSize;

	/* get_nodes define in ht_wrapper.c */
	nodes = get_nodes();

	disable_siblings = !CONFIG_LOGICAL_CPUS;

#if CONFIG_LOGICAL_CPUS == 1
	if(read_option(CMOS_VSTART_quad_core, CMOS_VLEN_quad_core, 0) != 0) { // 0 mean quad core
		disable_siblings = 1;
	}
#endif

	/* Assume that all node are same stepping, otherwise we can use use
	   nb_cfg_54 from bsp for all nodes */
	nb_cfg_54 = read_nb_cfg_54();

	ApicIdCoreIdSize = (cpuid_ecx(0x80000008) >> 12 & 0xf);
	if(ApicIdCoreIdSize) {
		siblings = ((1 << ApicIdCoreIdSize) - 1);
	} else {
		siblings = 3; //quad core
	}

	for (i = 0; i < nodes; i++) {
		cores_found = get_core_num_in_bsp(i);

		u32 jstart, jend;

		if (core_range == 2) {
			jstart = 1;
		} else {
			jstart = 0;
		}

		if (disable_siblings || (core_range==1)) {
			jend = 0;
		} else {
			jend = cores_found;
		}


		for (j = jstart; j <= jend; j++) {
			ap_apicid = i * (nb_cfg_54 ? (siblings + 1):1) + j * (nb_cfg_54 ? 1:64);

		#if (ENABLE_APIC_EXT_ID == 1) && (APIC_ID_OFFSET > 0)
			#if LIFT_BSP_APIC_ID == 0
			if( (i != 0) || (j != 0)) /* except bsp */
			#endif
				ap_apicid += APIC_ID_OFFSET;
		#endif

			if(ap_apicid == bsp_apicid) continue;

			process_ap(ap_apicid, gp);

		}
	}
}

/* FIXME: Duplicate of what is in lapic.h? */
static inline int lapic_remote_read(int apicid, int reg, u32 *pvalue)
{
	int timeout;
	u32 status;
	int result;
	lapic_wait_icr_idle();
	lapic_write(LAPIC_ICR2, SET_LAPIC_DEST_FIELD(apicid));
	lapic_write(LAPIC_ICR, LAPIC_DM_REMRD | (reg >> 4));
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


/* Use the LAPIC timer count register to hold each cores init status */
#define LAPIC_MSG_REG 0x380


#if FAM10_SET_FIDVID == 1
static void init_fidvid_ap(u32 bsp_apicid, u32 apicid, u32 nodeid, u32 coreid);
#endif

static inline __attribute__((always_inline)) void print_apicid_nodeid_coreid(u32 apicid, struct node_core_id id, const char *str)
{
		printk_debug("%s --- {	 APICID = %02x NODEID = %02x COREID = %02x} ---\n", str, apicid, id.nodeid, id.coreid);
}


static unsigned wait_cpu_state(u32 apicid, u32 state)
{
	u32 readback = 0;
	u32 timeout = 1;
	int loop = 4000000;
	while (--loop > 0) {
		if (lapic_remote_read(apicid, LAPIC_MSG_REG, &readback) != 0) continue;
		if ((readback & 0x3f) == state) {
			timeout = 0;
			break; //target cpu is in stage started
		}
	}
	if (timeout) {
		if (readback) {
			timeout = readback;
		}
	}

	return timeout;
}


static void wait_ap_started(u32 ap_apicid, void *gp )
{
	u32 timeout;
	timeout = wait_cpu_state(ap_apicid, 0x13); // started
	if(timeout) {
		print_initcpu8_nocr("* AP ", ap_apicid);
		print_initcpu(" didn't start timeout:", timeout);
	}
	else {
		print_initcpu8_nocr("AP started: ", ap_apicid);
	}
}


static void wait_all_aps_started(u32 bsp_apicid)
{
	for_each_ap(bsp_apicid, 0 , wait_ap_started, (void *)0);
}


static void wait_all_other_cores_started(u32 bsp_apicid)
{
	// all aps other than core0
	print_debug("started ap apicid: ");
	for_each_ap(bsp_apicid, 2 , wait_ap_started, (void *)0);
	print_debug("\n");
}


static void allow_all_aps_stop(u32 bsp_apicid)
{
	/* Called by the BSP to indicate AP can stop */

	/* FIXME Do APs use this?
	   Looks like wait_till_sysinfo_in_ram is used instead. */

	// allow aps to stop use 6 bits for state
	lapic_write(LAPIC_MSG_REG, (bsp_apicid << 24) | 0x14);
}


static void STOP_CAR_AND_CPU()
{
	disable_cache_as_ram(); // inline
	stop_this_cpu();
}


#ifndef MEM_TRAIN_SEQ
#define MEM_TRAIN_SEQ 0
#endif

#if RAMINIT_SYSINFO == 1
static u32 init_cpus(u32 cpu_init_detectedx ,struct sys_info *sysinfo)
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

	/* enable access pci conf via mmio*/
	set_pci_mmio_conf_reg();

	/* that is from initial apicid, we need nodeid and coreid
	   later */
	id = get_node_core_id_x();

	/* NB_CFG MSR is shared between cores, so we need make sure
	  core0 is done at first --- use wait_all_core0_started  */
	if(id.coreid == 0) {
		set_apicid_cpuid_lo(); /* only set it on core0 */
		set_EnableCf8ExtCfg(); /* only set it on core0 */
		#if (ENABLE_APIC_EXT_ID == 1)
		enable_apic_ext_id(id.nodeid);
		#endif
	}

	enable_lapic();


#if (ENABLE_APIC_EXT_ID == 1) && (APIC_ID_OFFSET > 0)
	u32 initial_apicid = get_initial_apicid();

	#if LIFT_BSP_APIC_ID == 0
	if( initial_apicid != 0 ) // other than bsp
	#endif
	{
		/* use initial apic id to lift it */
		u32 dword = lapic_read(LAPIC_ID);
		dword &= ~(0xff << 24);
		dword |= (((initial_apicid + APIC_ID_OFFSET) & 0xff) << 24);

		lapic_write(LAPIC_ID, dword);
	}

	#if LIFT_BSP_APIC_ID == 1
	bsp_apicid += APIC_ID_OFFSET;
	#endif

#endif

	/* get the apicid, it may be lifted already */
	apicid = lapicid();

	// show our apicid, nodeid, and coreid
	if( id.coreid==0 ) {
		if (id.nodeid!=0) //all core0 except bsp
			print_apicid_nodeid_coreid(apicid, id, " core0: ");
	}
	else { //all other cores
		print_apicid_nodeid_coreid(apicid, id, " corex: ");
	}


	if (cpu_init_detectedx) {
		print_apicid_nodeid_coreid(apicid, id, "\n\n\nINIT detected from ");
		print_debug("\nIssuing SOFT_RESET...\n");
		soft_reset();
	}

	if(id.coreid == 0) {
		if(!(warm_reset_detect(id.nodeid))) //FIXME: INIT is checked above but check for more resets?
			distinguish_cpu_resets(id.nodeid); // Also indicates we are started
	}

	// Mark the core as started.
	lapic_write(LAPIC_MSG_REG, (apicid << 24) | 0x13);


	if(apicid != bsp_apicid) {
#if FAM10_SET_FIDVID == 1
	#if (CONFIG_LOGICAL_CPUS == 1)  && (FAM10_SET_FIDVID_CORE0_ONLY == 1)
		// Run on all AP for proper FID/VID setup.
		if(id.coreid == 0 ) // only need set fid for core0
	#endif
		{
		// check warm(bios) reset to call stage2 otherwise do stage1
			if (warm_reset_detect(id.nodeid)) {
				printk_debug("init_fidvid_stage2 apicid: %02x\n", apicid);
				init_fidvid_stage2(apicid, id.nodeid);
			} else {
				printk_debug("init_fidvid_ap(stage1) apicid: %02x\n", apicid);
				init_fidvid_ap(bsp_apicid, apicid, id.nodeid, id.coreid);
			}
		}
#endif

		/* AP is ready, Wait for the BSP to get memory configured */
		/* FIXME: many cores spinning on node0 pci register seems to be bad.
		 * Why do we need to wait? These APs are just going to go sit in a hlt.
		 */
		//wait_till_sysinfo_in_ram();

		set_init_ram_access();

		STOP_CAR_AND_CPU();
		printk_debug("\nAP %02x should be halted but you are reading this....\n", apicid);
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


static void wait_all_core0_started(void)
{
	/* When core0 is started, it will distingush_cpu_resets
	  . So wait for that to finish */
	u32 i;
	u32 nodes = get_nodes();

	printk_debug("Wait all core0s started \n");
	for(i=1;i<nodes;i++) { // skip bsp, because it is running on bsp
		while(!is_core0_started(i)) {}
		print_initcpu8("  Core0 started on node: ", i);
	}
	printk_debug("Wait all core0s started done\n");
}
#if CONFIG_MAX_PHYSICAL_CPUS > 1
/**
 * void start_node(u32 node)
 *
 *  start the core0 in node, so it can generate HT packet to feature code.
 *
 * This function starts the AP nodes core0s. wait_all_core0_started() in
 * cache_as_ram_auto.c waits for all the AP to be finished before continuing
 * system init.
 */
static void start_node(u8 node)
{
	u32 val;

	/* Enable routing table */
	printk_debug("Start node %02x", node);

#if CAR_FAM10 == 1
	/* For CAR_FAM10 support, we need to set Dram base/limit for the new node */
	pci_write_config32(NODE_MP(node), 0x44, 0);
	pci_write_config32(NODE_MP(node), 0x40, 3);
#endif

	/* Allow APs to make requests (ROM fetch) */
	val=pci_read_config32(NODE_HT(node), 0x6c);
	val &= ~(1 << 1);
	pci_write_config32(NODE_HT(node), 0x6c, val);

	printk_debug(" done.\n");
}


/**
 * static void setup_remote_node(u32 node)
 *
 * Copy the BSP Adress Map to each AP.
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

	printk_debug("setup_remote_node: %02x", node);

	/* copy the default resource map from node 0 */
	for(i = 0; i < sizeof(pci_reg)/sizeof(pci_reg[0]); i++) {
		u32 value;
		u16 reg;
		reg = pci_reg[i];
		value = pci_read_config32(NODE_MP(0), reg);
		pci_write_config32(NODE_MP(node), reg, value);

	}
	printk_debug(" done\n");
}
#endif
/**
 * finalize_node_setup()
 *
 * Do any additional post HT init
 *
 * This could really be moved to cache_as_ram_auto.c since it really isn't HT init.
 */
void finalize_node_setup(struct sys_info *sysinfo)
{
	u8 i;
	u8 nodes = get_nodes();
	u32 reg;

#if RAMINIT_SYSINFO == 1
	/* read Node0 F0_0x64 bit [8:10] to find out SbLink # */
	reg = pci_read_config32(NODE_HT(0), 0x64);
	sysinfo->sblk = (reg>>8) & 7;
	sysinfo->sbbusn = 0;
	sysinfo->nodes = nodes;
	sysinfo->sbdn = get_sbdn(sysinfo->sbbusn);
#endif

	setup_link_trans_cntrl();

#if FAM10_SET_FIDVID == 1
	// Prep each node for FID/VID setup.
	prep_fid_change();
#endif

#if CONFIG_MAX_PHYSICAL_CPUS > 1
	/* Skip the BSP, start at node 1 */
	for(i=1; i<nodes; i++) {
		setup_remote_node(i);
		start_node(i);
	}
#endif
}

