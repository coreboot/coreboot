#ifndef AMD_EARLYMTRR_C
#define AMD_EARLYMTRR_C
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include "cpu/x86/mtrr/earlymtrr.c"


static void amd_early_mtrr_init(void)
{
	static const unsigned long mtrr_msrs[] = {
		/* fixed mtrr */
		0x250, 0x258, 0x259,
		0x268, 0x269, 0x26A,
		0x26B, 0x26C, 0x26D,
		0x26E, 0x26F,
		/* var mtrr */
		0x200, 0x201, 0x202, 0x203,
		0x204, 0x205, 0x206, 0x207,
		0x208, 0x209, 0x20A, 0x20B,
		0x20C, 0x20D, 0x20E, 0x20F,
		/* var iorr */
		0xC0010016, 0xC0010017, 0xC0010018, 0xC0010019,
		/* mem top */
		0xC001001A, 0xC001001D,
		/* NULL end of table */
		0
	};
	msr_t msr;

	/* wbinvd which is called in disable_cache() causes hangs on Opterons
	 * if there is no data in the cache.
	 * At this point we should not have the cache enabled so don't bother
	 * disabling it.
	 */
	/* disable_cache(); */
	do_early_mtrr_init(mtrr_msrs);

	/* Enable memory access for 0 - 1MB using top_mem */
	msr.hi = 0;
	msr.lo = (((CONFIG_LB_MEM_TOPK << 10) + TOP_MEM_MASK) & ~TOP_MEM_MASK);
	wrmsr(TOP_MEM, msr);

	/* Enable the MTRRs in SYSCFG */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrVarDramEn;
	wrmsr(SYSCFG_MSR, msr);

	enable_cache();
}

#endif /* AMD_EARLYMTRR_C */
