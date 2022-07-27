/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_X86_MTRR_H
#define CPU_X86_MTRR_H

#ifndef __ASSEMBLER__
#include <cpu/x86/msr.h>
#include <arch/cpu.h>
#endif

/*  These are the region types  */
#define MTRR_TYPE_UNCACHEABLE		0
#define MTRR_TYPE_WRCOMB		1
#define MTRR_TYPE_WRTHROUGH		4
#define MTRR_TYPE_WRPROT		5
#define MTRR_TYPE_WRBACK		6
#define MTRR_NUM_TYPES			7

#define MTRR_CAP_MSR			0x0fe

#define MTRR_CAP_PRMRR			(1 << 12)
#define MTRR_CAP_SMRR			(1 << 11)
#define MTRR_CAP_WC			(1 << 10)
#define MTRR_CAP_FIX			(1 << 8)
#define MTRR_CAP_VCNT			0xff

#define MTRR_DEF_TYPE_MSR		0x2ff
#define MTRR_DEF_TYPE_MASK		0xff
#define MTRR_DEF_TYPE_EN		(1 << 11)
#define MTRR_DEF_TYPE_FIX_EN		(1 << 10)

#define IA32_SMRR_PHYS_BASE		0x1f2
#define IA32_SMRR_PHYS_MASK		0x1f3
#define SMRR_PHYS_MASK_LOCK		(1 << 10)

/* Specific to model_6fx and model_1067x.
   These are named MSR_SMRR_PHYSBASE in the SDM. */
#define CORE2_SMRR_PHYS_BASE		0xa0
#define CORE2_SMRR_PHYS_MASK		0xa1

#define MTRR_PHYS_BASE(reg)		(0x200 + 2 * (reg))
#define MTRR_PHYS_MASK(reg)		(MTRR_PHYS_BASE(reg) + 1)
#define  MTRR_PHYS_MASK_VALID		(1 << 11)

#define NUM_FIXED_RANGES		88
#define RANGES_PER_FIXED_MTRR		8
#define MTRR_FIX_64K_00000		0x250
#define MTRR_FIX_16K_80000		0x258
#define MTRR_FIX_16K_A0000		0x259
#define MTRR_FIX_4K_C0000		0x268
#define MTRR_FIX_4K_C8000		0x269
#define MTRR_FIX_4K_D0000		0x26a
#define MTRR_FIX_4K_D8000		0x26b
#define MTRR_FIX_4K_E0000		0x26c
#define MTRR_FIX_4K_E8000		0x26d
#define MTRR_FIX_4K_F0000		0x26e
#define MTRR_FIX_4K_F8000		0x26f

#if !defined(__ASSEMBLER__)

#include <stdint.h>
#include <stddef.h>

/*
 * The MTRR code has some side effects that the callers should be aware for.
 * 1. The call sequence matters. x86_setup_mtrrs() calls
 *    x86_setup_fixed_mtrrs_no_enable() then enable_fixed_mtrrs() (equivalent
 *    of x86_setup_fixed_mtrrs()) then x86_setup_var_mtrrs(). If the callers
 *    want to call the components of x86_setup_mtrrs() because of other
 *    requirements the ordering should still preserved.
 * 2. enable_fixed_mtrr() will enable both variable and fixed MTRRs because
 *    of the nature of the global MTRR enable flag. Therefore, all direct
 *    or indirect callers of enable_fixed_mtrr() should ensure that the
 *    variable MTRR MSRs do not contain bad ranges.
 *
 * Note that this function sets up MTRRs for addresses above 4GiB.
 */
void x86_setup_mtrrs(void);
/*
 * x86_setup_mtrrs_with_detect() does the same thing as x86_setup_mtrrs(), but
 * it always dynamically detects the number of variable MTRRs available.
 */
void x86_setup_mtrrs_with_detect(void);
void x86_setup_mtrrs_with_detect_no_above_4gb(void);
/*
 * x86_setup_var_mtrrs() parameters:
 * address_bits - number of physical address bits supported by cpu
 * above4gb - if set setup MTRRs for addresses above 4GiB else ignore
 *            memory ranges above 4GiB
 */
void x86_setup_var_mtrrs(unsigned int address_bits, unsigned int above4gb);
void enable_fixed_mtrr(void);
/* Unhide Rd/WrDram bits and allow modification for AMD. */
void fixed_mtrrs_expose_amd_rwdram(void);
/* Hide Rd/WrDram bits and allow modification for AMD. */
void fixed_mtrrs_hide_amd_rwdram(void);
void x86_setup_fixed_mtrrs(void);
/* Set up fixed MTRRs but do not enable them. */
void x86_setup_fixed_mtrrs_no_enable(void);
void x86_mtrr_check(void);

/* Insert a temporary MTRR range for the duration of coreboot's runtime.
 * This function needs to be called after the first MTRR solution is derived. */
void mtrr_use_temp_range(uintptr_t begin, size_t size, int type);

static inline int get_var_mtrr_count(void)
{
	return rdmsr(MTRR_CAP_MSR).lo & MTRR_CAP_VCNT;
}

void set_var_mtrr(unsigned int reg, unsigned int base, unsigned int size,
	unsigned int type);
int get_free_var_mtrr(void);
void clear_all_var_mtrr(void);

asmlinkage void display_mtrrs(void);

struct var_mtrr_context {
	uint32_t max_var_mtrrs;
	uint32_t used_var_mtrrs;
	struct {
		msr_t base;
		msr_t mask;
	} mtrr[];
};

void var_mtrr_context_init(struct var_mtrr_context *ctx);
int var_mtrr_set(struct var_mtrr_context *ctx, uintptr_t addr, size_t size, int type);
void commit_mtrr_setup(const struct var_mtrr_context *ctx);
void postcar_mtrr_setup(void);

/* fms: find most significant bit set, stolen from Linux Kernel Source. */
static inline unsigned int fms(unsigned int x)
{
	unsigned int r;

	__asm__("bsrl %1,%0\n\t"
		"jnz 1f\n\t"
		"movl $0,%0\n"
		"1:" : "=r" (r) : "mr" (x));
	return r;
}

/* fls: find least significant bit set */
static inline unsigned int fls(unsigned int x)
{
	unsigned int r;

	__asm__("bsfl %1,%0\n\t"
		"jnz 1f\n\t"
		"movl $32,%0\n"
		"1:" : "=r" (r) : "mr" (x));
	return r;
}
#endif /* !defined(__ASSEMBLER__) */

/* Align up/down to next power of 2, suitable for assembler
   too. Range of result 256kB to 128MB is good enough here. */
#define _POW2_MASK(x)	((x>>1)|(x>>2)|(x>>3)|(x>>4)|(x>>5)| \
					(x>>6)|(x>>7)|(x>>8)|((1<<18)-1))
#define _ALIGN_UP_POW2(x)	((x + _POW2_MASK(x)) & ~_POW2_MASK(x))
#define _ALIGN_DOWN_POW2(x)	((x) & ~_POW2_MASK(x))

/* Calculate `4GiB - x` (e.g. absolute address for offset from 4GiB) */
#define _FROM_4G_TOP(x) ((0xffffffff - (x)) + 1)

/* At the end of romstage, low RAM 0..CACHE_TM_RAMTOP may be set
 * as write-back cacheable to speed up ramstage decompression.
 * Note MTRR boundaries, must be power of two.
 */
#define CACHE_TMP_RAMTOP (16<<20)

/* For ROM caching, generally, try to use the next power of 2. */
#define OPTIMAL_CACHE_ROM_SIZE _ALIGN_UP_POW2(CONFIG_ROM_SIZE)
#define OPTIMAL_CACHE_ROM_BASE _FROM_4G_TOP(OPTIMAL_CACHE_ROM_SIZE)
#if (OPTIMAL_CACHE_ROM_SIZE < CONFIG_ROM_SIZE) || \
    (OPTIMAL_CACHE_ROM_SIZE >= (2 * CONFIG_ROM_SIZE))
# error "Optimal CACHE_ROM_SIZE can't be derived, _POW2_MASK needs refinement."
#endif

/* Make sure it doesn't overlap CAR, though. If the gap between
   CAR and 4GiB is too small, make it at most the size of this
   gap. As we can't align up (might overlap again), align down
   to get a power of 2 again, for a single MTRR. */
#define CAR_END (CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE)
#if CAR_END > OPTIMAL_CACHE_ROM_BASE
# define CAR_CACHE_ROM_SIZE _ALIGN_DOWN_POW2(_FROM_4G_TOP(CAR_END))
#else
# define CAR_CACHE_ROM_SIZE OPTIMAL_CACHE_ROM_SIZE
#endif
#if ((CAR_CACHE_ROM_SIZE & (CAR_CACHE_ROM_SIZE - 1)) != 0)
# error "CAR CACHE_ROM_SIZE is not a power of 2, _POW2_MASK needs refinement."
#endif

/* Last but not least, most (if not all) chipsets have MMIO
   between 0xfe000000 and 0xff000000, so limit to 16MiB. */
#if CAR_CACHE_ROM_SIZE >= 16 << 20
# define CACHE_ROM_SIZE (16 << 20)
#else
# define CACHE_ROM_SIZE CAR_CACHE_ROM_SIZE
#endif

#define CACHE_ROM_BASE _FROM_4G_TOP(CACHE_ROM_SIZE)

#endif /* CPU_X86_MTRR_H */
