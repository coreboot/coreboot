#include <mainboard.h>
#include <types.h>
#include <lib.h>
#include <console.h>
#include <cpu.h>
#include <globalvars.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <cpu.h>
#include <amd/k8/k8.h>
/* We need to get a unified Id that works on dual, quad, whatever. We are going to leave
 * this as-is and fix later, working with AMD to improve it. 
 */

/* 2004.12 yhlu add dual core support */

//called by bus_cpu_scan too

/**
 * Return the value of Initial APIC ID CPU ID Low (InitApicIdCpuIdLo)—Bit 54. 
 * "When this bit is set, CpuId and NodeId[2:0] bit field positions are swapped in the APICID. "
 * BKDG page 374
 * @returns 1 if the bit is set. 
 */

unsigned int read_nb_cfg_54(void)
{
        struct msr msr;
        msr = rdmsr(NB_CFG_MSR);
        return ( ( msr.hi >> (54-32)) & 1);
}

/**
 * Return the initial CPU APIC ID.
 * @return APIC ID as read from cpuid_ebx, bits 24:27
 */
unsigned get_initial_apicid(void) 
{
	return ((cpuid_ebx(1) >> 24) & 0xf);
}

/**
 * Get the node and core id of the current CPU. This is formatted differently in the CPU as 
 * determined by the nb_cfg_54 parameter. See the note above for read_nb_cfg_54.
 * @returns a struct containing the node and core it. 
 */
//called by amd_siblings too
#define CORE_ID_BIT 1
#define NODE_ID_BIT 3
struct node_core_id get_node_core_id(void) 
{
	int nb_cfg_54 = read_nb_cfg_54();
	struct node_core_id id;
	//    get the apicid via cpuid(1) ebx[27:24]
	if( nb_cfg_54) {
                //   when NB_CFG[54] is set, nodeid = ebx[27:25], coreid = ebx[24]
                id.coreid = (cpuid_ebx(1) >> 24) & 0xf;
                id.nodeid = (id.coreid>>CORE_ID_BIT);
                id.coreid &= ((1<<CORE_ID_BIT)-1);
        } 
	else 
	{
                // when NB_CFG[54] is clear, nodeid = ebx[26:24], coreid = ebx[27]
                id.nodeid = (cpuid_ebx(1) >> 24) & 0xf;
                id.coreid = (id.nodeid>>NODE_ID_BIT);
                id.nodeid &= ((1<<NODE_ID_BIT)-1);
	}
	return id;
}

/**
 * Return the number of "other" cores in this CPU -- i.e. cores other than core 0
 * @return number of cores in addition to core 0
 */
unsigned int get_core_count(void)
{
	return (cpuid_ecx(0x80000008) & 0xff);
}
