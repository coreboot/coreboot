/* we will compile this into initram since some basic prototypes differ with same names on v2. Sigh. */
#include <mainboard.h>
#include <config.h>
#include <types.h>
#include <io.h>
#include <console.h>
#include <cpu.h>
#include <globalvars.h>
#include <lar.h>
#include <string.h>
#include <tables.h>
#include <lib.h>
#include <mc146818rtc.h>
#include <msr.h>
#include <mtrr.h>

void disable_var_mtrr(unsigned int reg)
{
	/* The invalid bit is kept in the mask so we simply
	 * clear the relevent mask register to disable a
	 * range.
	 */
	struct msr zero;
	zero.lo = zero.hi = 0;
	wrmsr(MTRRphysMask_MSR(reg), zero);
}

void stage1_set_var_mtrr(
	unsigned long reg, unsigned long base, unsigned long size, unsigned long type)

{
	/* Bit Bit 32-35 of MTRRphysMask should be set to 1 */
	/* FIXME: It only support 4G less range */
	struct msr basem, maskm;
	basem.lo = base | type;
	basem.hi = 0;
	wrmsr(MTRRphysBase_MSR(reg), basem);
	maskm.lo = ~(size - 1) | 0x800;
	maskm.hi = (1<<(CPU_ADDR_BITS-32))-1;
	wrmsr(MTRRphysMask_MSR(reg), maskm);
}

void set_var_mtrr_x(
        unsigned long reg, u32 base_lo, u32 base_hi, u32 size_lo, u32 size_hi, unsigned long type)

{
        /* Bit Bit 32-35 of MTRRphysMask should be set to 1 */
        struct msr basem, maskm;
        basem.lo = (base_lo & 0xfffff000) | type;
        basem.hi = base_hi & ((1<<(CPU_ADDR_BITS-32))-1);
        wrmsr(MTRRphysBase_MSR(reg), basem);
       	maskm.hi = (1<<(CPU_ADDR_BITS-32))-1;
	if(size_lo) {
	        maskm.lo = ~(size_lo - 1) | 0x800;
	} else {
		maskm.lo = 0x800;
		maskm.hi &= ~(size_hi - 1);
	}
        wrmsr(MTRRphysMask_MSR(reg), maskm);
}

void cache_cbmem(int type)
{
	/* Enable caching for 0 - 1MB using variable mtrr */
	disable_cache();
	stage1_set_var_mtrr(0, 0x00000000, CONFIG_CBMEMK << 10, type);
	enable_cache();
}

/* early_mtrr_init was only usable for ROMCC compiled code. It nukes CAR.
 * The only remaining purpose would be to enable ROM caching here instead of
 * in stage0.S and even that is debatable.
 */
#if 0
/* the fixed and variable MTTRs are power-up with random values,
 * clear them to MTRR_TYPE_UNCACHEABLE for safty.
 */
void do_early_mtrr_init(const unsigned long *mtrr_msrs)
{
	/* Precondition:
	 *   The cache is not enabled in cr0 nor in MTRRdefType_MSR
	 *   entry32.inc ensures the cache is not enabled in cr0
	 */
	struct msr msr;
	const unsigned long *msr_addr;

	/* Inialize all of the relevant msrs to 0 */
	msr.lo = 0;
	msr.hi = 0;
	unsigned long msr_nr;
	for(msr_addr = mtrr_msrs; (msr_nr = *msr_addr); msr_addr++) {
		wrmsr(msr_nr, msr);
	}

#warning fix the XIP bits in stage1_mtrr.c that  enable write through caching so we can do execute in place on the flash rom.
#if 0
#if defined(XIP_ROM_SIZE)
	/* enable write through caching so we can do execute in place
	 * on the flash rom.
	 */
	stage1_set_var_mtrr(1, XIP_ROM_BASE, XIP_ROM_SIZE, MTRR_TYPE_WRBACK);
#endif
#endif

	/* Set the default memory type and enable fixed and variable MTRRs 
	 */
	/* Enable Variable MTRRs */
	msr.hi = 0x00000000;
	msr.lo = 0x00000800;
	wrmsr(MTRRdefType_MSR, msr);
	
}

/**
 * Call this function early in stage1 to enable mtrrs, which will ensure 
 * caching of ROM 
 */
void early_mtrr_init(void)
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
#endif

int early_mtrr_init_detected(void)
{
	struct msr msr;
	/* See if MTRR's are enabled.
	 * a #RESET disables them while an #INIT
	 * preserves their state.  This works
	 * on both Intel and AMD cpus, at least
	 * according to the documentation.
	 */
	msr = rdmsr(MTRRdefType_MSR);
	return msr.lo & 0x00000800;
}
