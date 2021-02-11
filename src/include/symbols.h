/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SYMBOLS_H
#define __SYMBOLS_H

#include <types.h>

extern u8 _dram[];

#define REGION_SIZE(name) ((size_t)_##name##_size)

#define DECLARE_REGION(name)	\
	extern u8 _##name[];	\
	extern u8 _e##name[];	\
	extern u8 _##name##_size[];

/*
 * Regions can be declared optional if not all configurations provide them in
 * memlayout and you want code to be able to check for their existence at
 * runtime. Not every region that is architecture or platform-specific should
 * use this -- only declare regions optional if the code *accessing* them runs
 * both on configurations that have the region and those that don't.  That code
 * should then check (REGION_SIZE(name) != 0) before accessing it.
 */
#define DECLARE_OPTIONAL_REGION(name)	\
	__weak extern u8 _##name[];	\
	__weak extern u8 _e##name[];	\
	__weak extern u8 _##name##_size[];

DECLARE_REGION(sram)
DECLARE_OPTIONAL_REGION(timestamp)
DECLARE_REGION(preram_cbmem_console)
DECLARE_REGION(cbmem_init_hooks)
DECLARE_REGION(stack)
DECLARE_OPTIONAL_REGION(preram_cbfs_cache)
DECLARE_OPTIONAL_REGION(postram_cbfs_cache)
DECLARE_OPTIONAL_REGION(cbfs_cache)
DECLARE_REGION(cbfs_mcache)
DECLARE_REGION(fmap_cache)
DECLARE_REGION(tpm_tcpa_log)

#if ENV_ROMSTAGE && CONFIG(ASAN_IN_ROMSTAGE)
DECLARE_REGION(bss)
DECLARE_REGION(asan_shadow)
#endif

#if ENV_RAMSTAGE && CONFIG(ASAN_IN_RAMSTAGE)
DECLARE_REGION(data)
DECLARE_REGION(heap)
DECLARE_REGION(asan_shadow)
#endif

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
DECLARE_OPTIONAL_REGION(ttb_subtables)
DECLARE_REGION(dma_coherent)
DECLARE_REGION(soc_registers)
DECLARE_REGION(framebuffer)
DECLARE_REGION(pdpt)
DECLARE_OPTIONAL_REGION(opensbi)
DECLARE_OPTIONAL_REGION(bl31)
DECLARE_REGION(transfer_buffer)
DECLARE_OPTIONAL_REGION(watchdog_tombstone)

/* Returns true when pre-RAM symbols are known to the linker.
 * (Does not necessarily mean that the memory is accessible.) */
static inline int preram_symbols_available(void)
{
	return !ENV_X86 || ENV_ROMSTAGE_OR_BEFORE;
}

#endif /* __SYMBOLS_H */
