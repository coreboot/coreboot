/* 2004.12 yhlu add dual core support */
#include <mainboard.h>
#include <types.h>
#include <lib.h>
#include <console.h>
#include <globalvars.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <cpu.h>
#include <amd/k8/k8.h>
#include <mc146818rtc.h>
#include <spd.h>


#ifndef SET_NB_CFG_54
	#define SET_NB_CFG_54 1
#endif

/**
 * Given a nodeid, return the count of the number of cores in that node. 
 * Note there are two very similar functions that do almost the same thing. 
 * @param nodeid The nodeid
 * @returns The number of cores in that node. 
 */
unsigned int get_node_core_count(unsigned int nodeid)
{
	u32 dword;
	dword = pci_conf1_read_config32(PCI_BDF(0, 0x18+nodeid, 3), NORTHBRIDGE_CAP);
	dword >>= NBCAP_CmpCap_SHIFT;
	dword &= NBCAP_CmpCap_MASK;
	return dword;
}

/**
 * Set the "cpuid and node id" bits are swapped, from page 374: 
 * "When this bit is set, CpuId and NodeId[2:0] bit field positions 
 * are swapped in the APICID"
 * 0: APICID = {CpuId, NodeId[2:0]}
 * 1: APICID = {NodeId[2:0], CpuId}
 */
void set_apicid_cpuid_lo(void)
{
#if SET_NB_CFG_54 == 1
	//Why the OS thinks this is a good thing is not yet known. 
        struct msr msr;
        msr = rdmsr(NB_CFG_MSR);
        msr.hi |= (1<<(54-32)); // InitApicIdCpuIdLo
        wrmsr(NB_CFG_MSR, msr);
#endif
}

/**
 * Start the cores on a given node (cores > 0). It is important that MC4 and other accesses 
 * be directed to core 0, per the BKDG.  Note that this is pretty dual core specific. 
 * @param nodeid Node on which to start the cores
 */
void start_cores(unsigned nodeid)
{
	u32 dword;
	/* set PCI_BDF(0, 0x18+nodeid, 3), 0x44 bit 27 to redirect all MC4 
	 * accesses and error logging to core0
	 */
	dword = pci_conf1_read_config32(PCI_BDF(0, 0x18+nodeid, 3), MCA_NB_CONFIG);
	dword |= MNC_NBMCATOMSTCPUEN; // NbMcaToMstCpuEn bit
	pci_conf1_write_config32(PCI_BDF(0, 0x18+nodeid, 3), MCA_NB_CONFIG, dword);
	// set PCI_BDF(0, 0x18+nodeid, 0), 0x68 bit 5 to start core1
	dword = pci_conf1_read_config32(PCI_BDF(0, 0x18+nodeid, 0), HT_TRANSACTION_CONTROL);
	dword |= HTTC_CPU1_EN;
	pci_conf1_write_config32(PCI_BDF(0, 0x18+nodeid, 0), HT_TRANSACTION_CONTROL, dword);
}

/**
 * start cores on all nodes including BSP. This is assumed to be running on core 0 of node 0
 */
void start_all_cores(void)
{
	unsigned nodes;
	unsigned nodeid;
	int dual_core = 0;

	get_option(&dual_core, "dual_core");

	if (! dual_core)
		return;

        nodes = get_nodes();

        for(nodeid=0; nodeid<nodes; nodeid++) {
		if( get_node_core_count(nodeid) > 0) {
			start_cores(nodeid);
		}
	}

}

