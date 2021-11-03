/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _PATTRS_H_
#define _PATTRS_H_

#include <stdint.h>
#include <cpu/x86/msr.h>

enum {
	IACORE_MIN,
	IACORE_LFM,
	IACORE_MAX,
	IACORE_TURBO,
	IACORE_END
};

/*
 * The pattrs structure is a common place to stash pertinent information
 * about the processor or platform. Instead of going to the source (msrs, cpuid)
 * every time an attribute is needed use the pattrs structure.
 */
struct pattrs {
	msr_t platform_id;
	msr_t platform_info;
	int iacore_ratios[IACORE_END];
	int iacore_vids[IACORE_END];
	uint32_t cpuid;
	int revid;
	int stepping;
	const void *microcode_patch;
	int address_bits;
	unsigned int num_cpus;
	unsigned int bclk_khz;
};

/*
 * This is just to hide the abstraction w/o relying on how the underlying
 * storage is allocated.
 */
extern struct pattrs __global_pattrs;
static inline const struct pattrs *pattrs_get(void)
{
	return &__global_pattrs;
}

#endif /* _PATTRS_H_ */
