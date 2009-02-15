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

#if 1
#define BIOS_MTRRS 6
#define OS_MTRRS   2
#else
#define BIOS_MTRRS 8
#define OS_MTRRS   0
#endif
#define MTRRS (BIOS_MTRRS + OS_MTRRS)

/* fms: find most sigificant bit set, stolen from Linux Kernel Source. */
static inline u32 fms(u32 x)
{
	int r;

	__asm__("bsrl %1,%0\n\t"
			"jnz 1f\n\t"
			"movl $0,%0\n"
			"1:" : "=r" (r) : "g" (x));
	return r;
}

/* fls: find least sigificant bit set */
static inline u32 fls(u32 x)
{
	int r;

	__asm__("bsfl %1,%0\n\t"
			"jnz 1f\n\t"
			"movl $32,%0\n"
			"1:" : "=r" (r) : "g" (x));
	return r;
}

u32 stage1_resk(u64 value)
{
	u32 resultk;
	if (value < (1ULL << 42)) {
		resultk = value >> 10;
	}
	else {
		resultk = 0xffffffff;
	}
	return resultk;
}

/* Sets the entire fixed mtrr to a cache type. */
void stage1_set_fix_mtrr(u32 reg, u8 type)
{
	struct msr msr;

	/* Enable Modify Extended RdMem and WrMem settings */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, msr);

	msr.lo = (type << 24) | (type << 16) | (type << 8) | type;
	msr.hi = (type << 24) | (type << 16) | (type << 8) | type;
	wrmsr(reg, msr);

	/* Disable Modify Extended RdMem and WrMem settings */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, msr);
}

/* Set a variable MTRR, comes from linux kernel source
 * use stage1_range_to_mtrr() to set variable MTRRs.
 */
static void stage1_set_var_mtrr(u8 reg, u32 basek, u32 sizek,
								u8 type, u32 address_bits)
{
	struct msr base, mask;
	u32 address_mask_high;

	if (reg >= MTRRS) {
		printk(BIOS_ERR,"Requested MTRR is out of range!\n");
		return;
	}

	if (sizek == 0) {
		struct msr zero;
		zero.lo = zero.hi = 0;

		disable_cache();	/* disable/enable cache when setting MTRRs */

		/* The invalid bit is kept in the mask, so we simply clear the
		relevant mask register to disable a range. */
		wrmsr (MTRRphysMask_MSR(reg), zero);

		enable_cache();
		return;
	}


	address_mask_high = ((1u << (address_bits - 32u)) - 1u);

	base.hi = basek >> 22;
	base.lo  = basek << 10;

	printk(BIOS_SPEW, "ADDRESS_MASK_HIGH=%#x\n", address_mask_high);

	if (sizek < 4*1024*1024) {
		mask.hi = address_mask_high;
		mask.lo = ~((sizek << 10) -1);
	}
	else {
		mask.hi = address_mask_high & (~((sizek >> 22) -1));
		mask.lo = 0;
	}

	disable_cache();	/* disable/enable cache when setting MTRRs */

	/* Bit 32-35 of MTRRphysMask should be set to 1 */
	base.lo |= type;
	mask.lo |= 0x800;
	wrmsr (MTRRphysBase_MSR(reg), base);
	wrmsr (MTRRphysMask_MSR(reg), mask);

	enable_cache();
}

/* Set a memory range to variable MTRRs.
 * This handles the power of 2 alignment requirement.
 */
u8 stage1_range_to_mtrr(u8 reg, u32 range_startk, u32 range_sizek,
						u8 type, u32 address_bits)
{
	if (!range_sizek) {
		printk(BIOS_SPEW, "Zero-sized MTRR range @%dKB\n", range_startk);
		return reg;
	}

	if (reg >= BIOS_MTRRS) {
		printk(BIOS_ERR, "ERROR: Out of MTRRs for base: %4dMB, range: %dMB, type %s\n",
				range_startk >>10, range_sizek >> 10,
				(type==MTRR_TYPE_UNCACHEABLE)?"UC":
				((type==MTRR_TYPE_WRBACK)?"WB":"Other") );
		return reg;
	}

	while(range_sizek) {
		u32 max_align, align;
		u32 sizek;
		/* Compute the maximum size I can make a range */
		max_align = fls(range_startk);
		align = fms(range_sizek);
		if (align > max_align) {
			align = max_align;
		}
		sizek = 1 << align;
		printk(BIOS_DEBUG,"Setting variable MTRR %d, base: %dKB, range: %dKB, type %s\n",
			reg, range_startk, sizek,
			(type==MTRR_TYPE_UNCACHEABLE)?"UC":
			((type==MTRR_TYPE_WRBACK)?"WB":"Other")
			);
		stage1_set_var_mtrr(reg++, range_startk, sizek, type, address_bits);
		range_startk += sizek;
		range_sizek -= sizek;
		if (reg >= BIOS_MTRRS) {
			printk(BIOS_ERR, "Out of variable MTRRs!\n");
			break;
		}
	}
	return reg;
}

void cache_cbmem(int type)
{
	/* Enable caching for 0 - 1MB(CONFIG_CBMEMK) using variable mtrr */
	disable_cache();
	stage1_range_to_mtrr(0, 0, CONFIG_CBMEMK, MTRR_TYPE_WRBACK, CPU_ADDR_BITS);
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

#warning fix the XIP bits in stage1_mtrr.c that enable write through caching so we can do execute in place on the flash rom.
#if 0
#if defined(XIP_ROM_SIZE)
	/* enable write through caching so we can do execute in place
	 * on the flash rom.
	 */
	stage1_range_to_mtrr(1, 0, XIP_ROM_BASE >> 10, XIP_ROM_SIZE >> 10,
						MTRR_TYPE_WRBACK, CPU_ADDR_BITS);
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
