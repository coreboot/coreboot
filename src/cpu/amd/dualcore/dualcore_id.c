/* 2004.12 yhlu add dual core support */

#include <arch/cpu.h>
#include <cpu/amd/multicore.h>
#ifdef __PRE_RAM__
#include <cpu/amd/model_fxx_msr.h>
#endif

//called by bus_cpu_scan too
unsigned int read_nb_cfg_54(void)
{
        msr_t msr;
        msr = rdmsr(NB_CFG_MSR);
        return ( ( msr.hi >> (54-32)) & 1);
}

u32 get_initial_apicid(void)
{
	return ((cpuid_ebx(1) >> 24) & 0xf);
}

//called by amd_siblings too
#define CORE_ID_BIT 1
#define NODE_ID_BIT 3
struct node_core_id get_node_core_id(unsigned nb_cfg_54)
{
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

static inline unsigned get_core_num(void)
{
	return (cpuid_ecx(0x80000008) & 0xff);
}

struct node_core_id get_node_core_id_x(void)
{

	return get_node_core_id(read_nb_cfg_54()); // for pre_e0() nb_cfg_54 always be 0
}

