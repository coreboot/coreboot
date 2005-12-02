/* 2004.12 yhlu add dual core support */


#ifndef SET_NB_CFG_54
	#define SET_NB_CFG_54 1
#endif

#include "cpu/amd/dualcore/dualcore_id.c"

static inline unsigned get_core_num_in_bsp(unsigned nodeid)
{
	uint32_t dword;
	dword = pci_read_config32(PCI_DEV(0, 0x18+nodeid, 3), 0xe8);
	dword >>= 12;
	dword &= 3;
	return dword;
}

#if SET_NB_CFG_54 == 1
static inline uint8_t set_apicid_cpuid_lo(void)
{
        if(is_cpu_pre_e0()) return 0; // pre_e0 can not be set

        // set the NB_CFG[54]=1; why the OS will be happy with that ???
        msr_t msr;
        msr = rdmsr(NB_CFG_MSR);
        msr.hi |= (1<<(54-32)); // InitApicIdCpuIdLo
        wrmsr(NB_CFG_MSR, msr);

        return 1;
}
#else

static inline void set_apicid_cpuid_lo(void) { }

#endif

static inline void real_start_other_core(unsigned nodeid)
{
	uint32_t dword;
	// set PCI_DEV(0, 0x18+nodeid, 3), 0x44 bit 27 to redirect all MC4 accesses and error logging to core0
	dword = pci_read_config32(PCI_DEV(0, 0x18+nodeid, 3), 0x44);
	dword |= 1<<27; // NbMcaToMstCpuEn bit
	pci_write_config32(PCI_DEV(0, 0x18+nodeid, 3), 0x44, dword);
	// set PCI_DEV(0, 0x18+nodeid, 0), 0x68 bit 5 to start core1
	dword = pci_read_config32(PCI_DEV(0, 0x18+nodeid, 0), 0x68);
	dword |= 1<<5;
	pci_write_config32(PCI_DEV(0, 0x18+nodeid, 0), 0x68, dword);
}

//it is running on core0 of node0
static inline void start_other_cores(void)
{
	unsigned nodes;
	unsigned nodeid;

        if(read_option(CMOS_VSTART_dual_core, CMOS_VLEN_dual_core, 0) != 0)  { // disable dual_core
                return;
        }

        nodes = get_nodes();

        for(nodeid=0; nodeid<nodes; nodeid++) {
		if( get_core_num_in_bsp(nodeid) > 0) {
			real_start_other_core(nodeid);
		}
	}

}
#if USE_DCACHE_RAM == 0
static void do_k8_init_and_stop_secondaries(void)
{
	struct node_core_id id;
	device_t dev;
	unsigned apicid;
	unsigned max_siblings;
	msr_t msr;
	
	/* Skip this if there was a built in self test failure */

	if (is_cpu_pre_e0()) {
		id.nodeid = lapicid() & 0x7;
		id.coreid = 0;
	} else {
		/* Which cpu are we on? */
		id = get_node_core_id_x();

		/* Set NB_CFG_MSR
		 * Linux expect the core to be in the least signficant bits.
		 */
		msr = rdmsr(NB_CFG_MSR);
		msr.hi |= (1<<(54-32)); // InitApicIdCpuIdLo
		wrmsr(NB_CFG_MSR, msr);
	}

	/* For now assume all cpus have the same number of siblings */
	max_siblings = (cpuid_ecx(0x80000008) & 0xff) + 1;

	/* Enable extended apic ids */
	device_t dev_f0 = PCI_DEV(0, 0x18+id.nodeid, 0);
	unsigned val = pci_read_config32(dev_f0, 0x68);
	val |= (1 << 18) | (1 << 17);
	pci_write_config32(dev_f0, 0x68, val);

	/* Set the lapicid */
        #if (ENABLE_APIC_EXT_ID == 1)
                unsigned initial_apicid = get_initial_apicid();
                #if LIFT_BSP_APIC_ID == 0
                if( initial_apicid != 0 ) // other than bsp
                #endif
                {
                                /* use initial apic id to lift it */
                                uint32_t dword = lapic_read(LAPIC_ID);
                                dword &= ~(0xff<<24);
                                dword |= (((initial_apicid + APIC_ID_OFFSET) & 0xff)<<24);

                                lapic_write(LAPIC_ID, dword);
                }

                #if LIFT_BSP_APIC_ID == 1
                bsp_apicid += APIC_ID_OFFSET;
                #endif

        #endif


	/* Remember the cpuid */
	if (id.coreid == 0) {
		dev = PCI_DEV(0, 0x18 + id.nodeid, 2);
		pci_write_config32(dev, 0x9c, cpuid_eax(1));	
	}
	
	/* Maybe call distinguish_cpu_resets only on the last core? */
	distinguish_cpu_resets(id.nodeid);
	if (!boot_cpu()) {
		stop_this_cpu();
	}
}

static void k8_init_and_stop_secondaries(void)
{
	/* This doesn't work with Cache As Ram because it messes with 
	   the MTRR state, which breaks the init detection.
	   do_k8_init_and_stop_secondaries should be usable by CAR code.
	*/

	int init_detected;

	init_detected = early_mtrr_init_detected();
	amd_early_mtrr_init();

	enable_lapic();
	init_timer();
	if (init_detected) {
		asm volatile ("jmp __cpu_reset");
	}

	do_k8_init_and_stop_secondaries();
}

#endif
