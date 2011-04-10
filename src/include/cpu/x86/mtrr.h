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
void enable_fixed_mtrr(void);
void x86_setup_var_mtrrs(unsigned int address_bits, unsigned int above4gb);
void x86_setup_mtrrs(unsigned address_bits);
int x86_mtrr_check(void);
void set_var_mtrr_resource(void *gp, struct device *dev, struct resource *res);
void x86_setup_fixed_mtrrs(void);
#endif

/* Validate CONFIG_XIP_ROM_SIZE and CONFIG_XIP_ROM_BASE */
#if defined(CONFIG_XIP_ROM_SIZE) && !defined(CONFIG_XIP_ROM_BASE)
# error "CONFIG_XIP_ROM_SIZE without CONFIG_XIP_ROM_BASE"
#endif
#if defined(CONFIG_XIP_ROM_BASE) && !defined(CONFIG_XIP_ROM_SIZE)
# error "CONFIG_XIP_ROM_BASE without CONFIG_XIP_ROM_SIZE"
#endif
#if !defined(CONFIG_RAMTOP)
# error "CONFIG_RAMTOP not defined"
#endif

#if defined(CONFIG_XIP_ROM_SIZE) && ((CONFIG_XIP_ROM_SIZE & (CONFIG_XIP_ROM_SIZE -1)) != 0)
# error "CONFIG_XIP_ROM_SIZE is not a power of 2"
#endif
#if defined(CONFIG_XIP_ROM_SIZE) && ((CONFIG_XIP_ROM_BASE % CONFIG_XIP_ROM_SIZE) != 0)
# error "CONFIG_XIP_ROM_BASE is not a multiple of CONFIG_XIP_ROM_SIZE"
#endif

#if (CONFIG_RAMTOP & (CONFIG_RAMTOP -1)) != 0
# error "CONFIG_RAMTOP must be a power of 2"
#endif


#if !defined (__ASSEMBLER__)
#if defined(CONFIG_XIP_ROM_SIZE)
# if defined(CONFIG_TINY_BOOTBLOCK) && CONFIG_TINY_BOOTBLOCK
   extern unsigned long AUTO_XIP_ROM_BASE;
#  define REAL_XIP_ROM_BASE AUTO_XIP_ROM_BASE
# else
#  define REAL_XIP_ROM_BASE CONFIG_XIP_ROM_BASE
# endif
#endif

#endif

#endif /* CPU_X86_MTRR_H */
