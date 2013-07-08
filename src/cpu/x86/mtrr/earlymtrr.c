#ifndef EARLYMTRR_C
#define EARLYMTRR_C
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>

static void set_var_mtrr(
	unsigned reg, unsigned base, unsigned size, unsigned type)

{
	/* Bit Bit 32-35 of MTRRphysMask should be set to 1 */
	/* FIXME: It only support 4G less range */
	msr_t basem, maskm;
	basem.lo = base | type;
	basem.hi = 0;
	wrmsr(MTRRphysBase_MSR(reg), basem);
	maskm.lo = ~(size - 1) | MTRRphysMaskValid;
	maskm.hi = (1 << (CONFIG_CPU_ADDR_BITS - 32)) - 1;
	wrmsr(MTRRphysMask_MSR(reg), maskm);
}

#if !defined(CONFIG_CACHE_AS_RAM) || !CONFIG_CACHE_AS_RAM
static void cache_ramstage(void)
{
	/* Enable caching for lower 1MB and ram stage using variable mtrr */
	disable_cache();
	set_var_mtrr(0, 0x00000000, CONFIG_RAMTOP, MTRR_TYPE_WRBACK);
	enable_cache();
}

const int addr_det = 0;

/* the fixed and variable MTTRs are power-up with random values,
 * clear them to MTRR_TYPE_UNCACHEABLE for safety.
 */
static void do_early_mtrr_init(const unsigned long *mtrr_msrs)
{
	/* Precondition:
	 *   The cache is not enabled in cr0 nor in MTRRdefType_MSR
	 *   entry32.inc ensures the cache is not enabled in cr0
	 */
	msr_t msr;
	const unsigned long *msr_addr;

	/* Initialize all of the relevant msrs to 0 */
	msr.lo = 0;
	msr.hi = 0;
	unsigned long msr_nr;
	for(msr_addr = mtrr_msrs; (msr_nr = *msr_addr); msr_addr++) {
		wrmsr(msr_nr, msr);
	}

#if defined(CONFIG_XIP_ROM_SIZE)
	/* enable write through caching so we can do execute in place
	 * on the flash rom.
	 * Determine address by calculating the XIP_ROM_SIZE sized area with
	 * XIP_ROM_SIZE alignment that contains the global variable defined above;
	 */
        unsigned long f = (unsigned long)&addr_det & ~(CONFIG_XIP_ROM_SIZE - 1);
	set_var_mtrr(1, f, CONFIG_XIP_ROM_SIZE, MTRR_TYPE_WRBACK);
#endif

	/* Set the default memory type and enable fixed and variable MTRRs
	 */
	/* Enable Variable MTRRs */
	msr.hi = 0x00000000;
	msr.lo = 0x00000800;
	wrmsr(MTRRdefType_MSR, msr);

}

static inline void early_mtrr_init(void)
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
		/* NULL end of table */
		0
	};
	disable_cache();
	do_early_mtrr_init(mtrr_msrs);
	enable_cache();
}

static inline int early_mtrr_init_detected(void)
{
	msr_t msr;
	/* See if MTRR's are enabled.
	 * a #RESET disables them while an #INIT
	 * preserves their state.  This works
	 * on both Intel and AMD cpus, at least
	 * according to the documentation.
	 */
	msr = rdmsr(MTRRdefType_MSR);
	return msr.lo & MTRRdefTypeEn;
}
#endif

#endif /* EARLYMTRR_C */
