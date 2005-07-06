/* 2004.12 yhlu add dual core support */

#include <arch/cpu.h>
#include "cpu/amd/model_fxx/model_fxx_msr.h"

static inline unsigned get_node_id(void) {
	unsigned nodeid;
	//    get the apicid via cpuid(1) ebx[27:24]
        nodeid = (cpuid_ebx(1) >> 24) & 0x7;
	return nodeid;
}

