#if CONFIG_HAVE_OPTION_TABLE
#include "option_table.h"
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
	u32 siblings = 0;
	u32 disable_siblings;
	u32 e0_later_single_core;
	u32 nb_cfg_54;
	int i, j;

	/* get_nodes define in in_coherent_ht.c */
	nodes = get_nodes();

	if (!CONFIG_LOGICAL_CPUS ||
	    read_option(multi_core, 0) != 0) {	// 0 means multi core
		disable_siblings = 1;
	} else {
		disable_siblings = 0;
	}

	/* here I assume that all node are same stepping, otherwise we can use use nb_cfg_54 from bsp for all nodes */
	nb_cfg_54 = read_nb_cfg_54();

	for (i = 0; i < nodes; i++) {
		e0_later_single_core = 0;
		j = ((pci_read_config32(PCI_DEV(0, 0x18 + i, 3), 0xe8) >> 12) &
		     3);
		if (nb_cfg_54) {
			if (j == 0) {	// if it is single core, we need to increase siblings for apic calculation
#if !CONFIG_K8_REV_F_SUPPORT
				e0_later_single_core = is_e0_later_in_bsp(i);	// single core
#else
				e0_later_single_core = is_cpu_f0_in_bsp(i);	// We can read cpuid(1) from Func3
#endif
			}
			if (e0_later_single_core) {
				j = 1;
			}
		}
		siblings = j;

		u32 jstart, jend;

		if (core_range == 2) {
			jstart = 1;
		} else {
			jstart = 0;
		}

		if (e0_later_single_core || disable_siblings
		    || (core_range == 1)) {
			jend = 0;
		} else {
			jend = siblings;
		}

		for (j = jstart; j <= jend; j++) {
			ap_apicid =
			    i * (nb_cfg_54 ? (siblings + 1) : 1) +
			    j * (nb_cfg_54 ? 1 : 8);

#if CONFIG_ENABLE_APIC_EXT_ID
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

#define LAPIC_MSG_REG 0x380

#if CONFIG_SET_FIDVID
static void init_fidvid_ap(u32 bsp_apicid, u32 apicid);
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
	int loop = 2000000;
	while (--loop > 0) {
		if (lapic_remote_read(apicid, LAPIC_MSG_REG, &readback) != 0)
			continue;
		if ((readback & 0xff) == state) {
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
	timeout = wait_cpu_state(ap_apicid, 0x33);	// started
	printk(BIOS_DEBUG, "* AP %02x", ap_apicid);
	if (timeout) {
		printk(BIOS_DEBUG, " timed out:%08x\n", timeout);
	} else {
		printk(BIOS_DEBUG, "started\n");
	}
}

void wait_all_aps_started(u32 bsp_apicid)
{
	for_each_ap(bsp_apicid, 0, wait_ap_started, (void *)0);
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
	// allow aps to stop

	lapic_write(LAPIC_MSG_REG, (bsp_apicid << 24) | 0x44);
}

static void STOP_CAR_AND_CPU(void)
{
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
#if CONFIG_ENABLE_APIC_EXT_ID
		enable_apic_ext_id(id.nodeid);
#endif
	}

	enable_lapic();
	//      init_timer(); // We need TMICT to pass msg for FID/VID change

#if CONFIG_ENABLE_APIC_EXT_ID
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

#if 0
	// show our apicid, nodeid, and coreid
	if (id.coreid == 0) {
		if (id.nodeid != 0)	//all core0 except bsp
			print_apicid_nodeid_coreid(apicid, id, " core0: ");
	} else {		//all other cores
		print_apicid_nodeid_coreid(apicid, id, " corex: ");
	}
#endif

	if (cpu_init_detectedx) {
		print_apicid_nodeid_coreid(apicid, id,
					   "\n\n\nINIT detected from ");
		printk(BIOS_DEBUG, "\nIssuing SOFT_RESET...\n");
		soft_reset();
	}

	if (id.coreid == 0) {
		distinguish_cpu_resets(id.nodeid);
//            start_other_core(id.nodeid); // start second core in first cpu, only allowed for nb_cfg_54 is not set
	}
	//here don't need to wait
	lapic_write(LAPIC_MSG_REG, (apicid << 24) | 0x33);	// mark the cpu is started

	if (apicid != bsp_apicid) {
		u32 timeout = 1;
		u32 loop = 100;

#if CONFIG_SET_FIDVID
#if CONFIG_LOGICAL_CPUS && CONFIG_SET_FIDVID_CORE0_ONLY
		if (id.coreid == 0)	// only need set fid for core0
#endif
			init_fidvid_ap(bsp_apicid, apicid);
#endif

		// We need to stop the CACHE as RAM for this CPU, really?
		while (timeout && (loop-- > 0)) {
			timeout = wait_cpu_state(bsp_apicid, 0x44);
		}
		if (timeout) {
			printk(BIOS_DEBUG,
			       "while waiting for BSP signal to STOP, timeout in ap %02x\n",
			       apicid);
		}
		lapic_write(LAPIC_MSG_REG, (apicid << 24) | 0x44);	// bsp can not check it before stop_this_cpu
		set_var_mtrr(0, 0x00000000, CONFIG_RAMTOP, MTRR_TYPE_WRBACK);
#if CONFIG_MEM_TRAIN_SEQ == 1
		train_ram_on_node(id.nodeid, id.coreid, sysinfo,
				  (unsigned)STOP_CAR_AND_CPU);
#endif

		STOP_CAR_AND_CPU();
	}

	return bsp_apicid;
}

static u32 is_core0_started(u32 nodeid)
{
	u32 htic;
	device_t device;
	device = PCI_DEV(0, 0x18 + nodeid, 0);
	htic = pci_read_config32(device, HT_INIT_CONTROL);
	htic &= HTIC_INIT_Detect;
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
