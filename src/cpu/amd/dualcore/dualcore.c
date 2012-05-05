/* 2004.12 yhlu add dual core support */

#include "cpu/amd/dualcore/dualcore_id.c"
#include <pc80/mc146818rtc.h>
#if CONFIG_HAVE_OPTION_TABLE
#include "option_table.h"
#endif

static inline unsigned get_core_num_in_bsp(unsigned nodeid)
{
	uint32_t dword;
	dword = pci_read_config32(PCI_DEV(0, 0x18+nodeid, 3), 0xe8);
	dword >>= 12;
	dword &= 3;
	return dword;
}

static inline uint8_t set_apicid_cpuid_lo(void)
{
#if !CONFIG_K8_REV_F_SUPPORT
        if(is_cpu_pre_e0()) return 0; // pre_e0 can not be set
#endif

        // set the NB_CFG[54]=1; why the OS will be happy with that ???
        msr_t msr;
        msr = rdmsr(NB_CFG_MSR);
        msr.hi |= (1<<(54-32)); // InitApicIdCpuIdLo
        wrmsr(NB_CFG_MSR, msr);

        return 1;
}

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

	if (read_option(multi_core, 0))  {
		return; // disable multi_core
	}

        nodes = get_nodes();

        for(nodeid=0; nodeid<nodes; nodeid++) {
		if( get_core_num_in_bsp(nodeid) > 0) {
			real_start_other_core(nodeid);
		}
	}

}


