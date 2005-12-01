/* 2004.12 yhlu add dual core support */

#include <arch/cpu.h>
#include "cpu/amd/model_fxx/model_fxx_msr.h"

static inline unsigned int read_nb_cfg_54(void)
{
        msr_t msr;
        msr = rdmsr(NB_CFG_MSR);
        return ( ( msr.hi >> (54-32)) & 1);
}

struct node_core_id {
	unsigned nodeid:8;
	unsigned coreid:8;
};

static inline unsigned get_initial_apicid(void)
{
	return ((cpuid_ebx(1) >> 24) & 0xf);
}

static inline struct node_core_id get_node_core_id(unsigned nb_cfg_54) {
	struct node_core_id id;
	//    get the apicid via cpuid(1) ebx[27:24]
	if( nb_cfg_54) {
                //   when NB_CFG[54] is set, nodeid = ebx[27:25], coreid = ebx[24]
                id.coreid = (cpuid_ebx(1) >> 24) & 0xf;
                id.nodeid = (id.coreid>>1);
                id.coreid &= 1;
        } 
	else 
	{
                // when NB_CFG[54] is clear, nodeid = ebx[26:24], coreid = ebx[27]
                id.nodeid = (cpuid_ebx(1) >> 24) & 0xf;
                id.coreid = (id.nodeid>>3);
                id.nodeid &= 7;
	}
	return id;
}

static inline unsigned get_core_num(void)
{
	return (cpuid_ecx(0x80000008) & 0xff);
}

static inline struct node_core_id get_node_core_id_x(void) {
	return get_node_core_id( read_nb_cfg_54() ); 
}

