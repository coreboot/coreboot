#ifndef AMD_EARLYMTRR_C
#define AMD_EARLYMTRR_C
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include "cpu/x86/mtrr/earlymtrr.c"

/* the fixed and variable MTTRs are power-up with random values,
 * clear them to MTRR_TYPE_UNCACHEABLE for safty.
 */             
static void do_amd_early_mtrr_init(const unsigned long *mtrr_msrs)
{               
        /* Precondition:
         *   The cache is not enabled in cr0 nor in MTRRdefType_MSR
         *   entry32.inc ensures the cache is not enabled in cr0
         */
        msr_t msr;
        const unsigned long *msr_addr;
#if 0
        /* Enable the access to AMD RdDram and WrDram extension bits */
        msr = rdmsr(SYSCFG_MSR);
        msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
        wrmsr(SYSCFG_MSR, msr);
#endif

        /* Inialize all of the relevant msrs to 0 */
        msr.lo = 0;
        msr.hi = 0;
        unsigned long msr_nr;
        for(msr_addr = mtrr_msrs; (msr_nr = *msr_addr); msr_addr++) {
                wrmsr(msr_nr, msr);
        }
#if 0
        /* Disable the access to AMD RdDram and WrDram extension bits */
        msr = rdmsr(SYSCFG_MSR);
        msr.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
        wrmsr(SYSCFG_MSR, msr);
#endif

        /* Enable memory access for 0 - 1MB using top_mem */
        msr.hi = 0;
        msr.lo = (((CONFIG_RAMTOP) + TOP_MEM_MASK) & ~TOP_MEM_MASK);
        wrmsr(TOP_MEM, msr);

#if defined(CONFIG_XIP_ROM_SIZE)
        /* enable write through caching so we can do execute in place
         * on the flash rom.
         */
        set_var_mtrr(1, CONFIG_XIP_ROM_BASE, CONFIG_XIP_ROM_SIZE, MTRR_TYPE_WRBACK);
#endif

        /* Set the default memory type and enable fixed and variable MTRRs 
         */
        /* Enable Variable MTRRs */
        msr.hi = 0x00000000;
        msr.lo = 0x00000800;
        wrmsr(MTRRdefType_MSR, msr);

        /* Enable the MTRRs in SYSCFG */
        msr = rdmsr(SYSCFG_MSR);
        msr.lo |= SYSCFG_MSR_MtrrVarDramEn;
        wrmsr(SYSCFG_MSR, msr);
        
}

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

	/* wbinvd which is called in disable_cache() causes hangs on Opterons
	 * if there is no data in the cache.
	 * At this point we should not have the cache enabled so don't bother
	 * disabling it.
	 */
	/* disable_cache(); */
	do_amd_early_mtrr_init(mtrr_msrs);

	enable_cache();
}

#endif /* AMD_EARLYMTRR_C */
