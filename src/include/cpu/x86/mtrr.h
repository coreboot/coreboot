#ifndef CPU_X86_MTRR_H
#define CPU_X86_MTRR_H

/*  These are the region types  */
#define MTRR_TYPE_UNCACHEABLE 0
#define MTRR_TYPE_WRCOMB     1
/*#define MTRR_TYPE_         2*/
/*#define MTRR_TYPE_         3*/
#define MTRR_TYPE_WRTHROUGH  4
#define MTRR_TYPE_WRPROT     5
#define MTRR_TYPE_WRBACK     6
#define MTRR_NUM_TYPES       7

#define MTRRcap_MSR     0x0fe
#define MTRRdefType_MSR 0x2ff

#define MTRRdefTypeEn		(1 << 11)
#define MTRRdefTypeFixEn	(1 << 10)

#define SMRRphysBase_MSR 0x1f2
#define SMRRphysMask_MSR 0x1f3

#define MTRRphysBase_MSR(reg) (0x200 + 2 * (reg))
#define MTRRphysMask_MSR(reg) (0x200 + 2 * (reg) + 1)

#define MTRRphysMaskValid	(1 << 11)

#define NUM_FIXED_RANGES 88
#define MTRRfix64K_00000_MSR 0x250
#define MTRRfix16K_80000_MSR 0x258
#define MTRRfix16K_A0000_MSR 0x259
#define MTRRfix4K_C0000_MSR 0x268
#define MTRRfix4K_C8000_MSR 0x269
#define MTRRfix4K_D0000_MSR 0x26a
#define MTRRfix4K_D8000_MSR 0x26b
#define MTRRfix4K_E0000_MSR 0x26c
#define MTRRfix4K_E8000_MSR 0x26d
#define MTRRfix4K_F0000_MSR 0x26e
#define MTRRfix4K_F8000_MSR 0x26f

#if !defined (__ASSEMBLER__) && !defined(__PRE_RAM__)
#include <device/device.h>
/* You should almost NEVER use this function.
 * N.B. We worked on a lot of ways to make this continue as static,
 * but just making it available ended up being the simplest solution.
 */
void set_var_mtrr(
	unsigned int reg, unsigned long basek, unsigned long sizek,
	unsigned char type, unsigned address_bits);
void enable_fixed_mtrr(void);
void x86_setup_var_mtrrs(unsigned int address_bits, unsigned int above4gb);
void x86_setup_mtrrs(void);
int x86_mtrr_check(void);
void set_var_mtrr_resource(void *gp, struct device *dev, struct resource *res);
void x86_setup_fixed_mtrrs(void);
/* Set up fixed MTRRs but do not enable them. */
void x86_setup_fixed_mtrrs_no_enable(void);
#endif

#if !defined(CONFIG_RAMTOP)
# error "CONFIG_RAMTOP not defined"
#endif

#if ((CONFIG_XIP_ROM_SIZE & (CONFIG_XIP_ROM_SIZE -1)) != 0)
# error "CONFIG_XIP_ROM_SIZE is not a power of 2"
#endif

#if ((CONFIG_CACHE_ROM_SIZE & (CONFIG_CACHE_ROM_SIZE -1)) != 0)
# error "CONFIG_CACHE_ROM_SIZE is not a power of 2"
#endif

#define CACHE_ROM_BASE	(((1<<20) - (CONFIG_CACHE_ROM_SIZE>>12))<<12)

#if (CONFIG_RAMTOP & (CONFIG_RAMTOP - 1)) != 0
# error "CONFIG_RAMTOP must be a power of 2"
#endif

#endif /* CPU_X86_MTRR_H */
