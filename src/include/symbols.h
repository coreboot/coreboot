/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __SYMBOLS_H
#define __SYMBOLS_H

#include <types.h>

extern u8 _dram[];

#define REGION_SIZE(name) (_e##name - _##name)

#define DECLARE_REGION(name)	\
	extern u8 _##name[];	\
	extern u8 _e##name[];

DECLARE_REGION(sram)
DECLARE_REGION(timestamp)
DECLARE_REGION(preram_cbmem_console)
DECLARE_REGION(cbmem_init_hooks)
DECLARE_REGION(stack)
DECLARE_REGION(preram_cbfs_cache)
DECLARE_REGION(postram_cbfs_cache)
DECLARE_REGION(cbfs_cache)
DECLARE_REGION(fmap_cache)
DECLARE_REGION(tpm_tcpa_log)

/* Regions for execution units. */

DECLARE_REGION(payload)
/* "program" always refers to the current execution unit. */
DECLARE_REGION(program)
/* _<stage>_size is always the maximum amount allocated in memlayout, whereas
   _program_size gives the actual memory footprint *used* by current stage. */
DECLARE_REGION(decompressor)
DECLARE_REGION(bootblock)
DECLARE_REGION(verstage)
DECLARE_REGION(romstage)
DECLARE_REGION(postcar)
DECLARE_REGION(ramstage)

/* Arch-specific, move to <arch/symbols.h> if they become too many. */

DECLARE_REGION(pagetables)
DECLARE_REGION(ttb)
DECLARE_REGION(ttb_subtables)
DECLARE_REGION(dma_coherent)
DECLARE_REGION(soc_registers)
DECLARE_REGION(framebuffer)
DECLARE_REGION(pdpt)
DECLARE_REGION(opensbi)
DECLARE_REGION(bl31)

/*
 * Put this into a .c file accessing a linker script region to mark that region
 * as "optional". If it is defined in memlayout.ld (or anywhere else), the
 * values from that definition will be used. If not, start, end and size will
 * all evaluate to 0. (We can't explicitly assign the symbols to 0 in the
 * assembly due to https://sourceware.org/bugzilla/show_bug.cgi?id=1038.)
 */
#define DECLARE_OPTIONAL_REGION(name) asm (".weak _" #name ", _e" #name)

/* Returns true when pre-RAM symbols are known to the linker.
 * (Does not necessarily mean that the memory is accessible.) */
static inline int preram_symbols_available(void)
{
	return !CONFIG(ARCH_X86) || ENV_ROMSTAGE_OR_BEFORE;
}

#endif /* __SYMBOLS_H */
