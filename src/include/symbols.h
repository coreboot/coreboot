/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SYMBOLS_H
#define __SYMBOLS_H

#include <types.h>

extern u8 _sram[];
extern u8 _esram[];
#define _sram_size (_esram - _sram)

extern u8 _dram[];

extern u8 _timestamp[];
extern u8 _etimestamp[];
#define _timestamp_size	(_etimestamp - _timestamp)

extern u8 _preram_cbmem_console[];
extern u8 _epreram_cbmem_console[];
#define _preram_cbmem_console_size \
		(_epreram_cbmem_console - _preram_cbmem_console)

extern u8 _cbmem_init_hooks[];
extern u8 _ecbmem_init_hooks[];
#define _cbmem_init_hooks_size (_ecbmem_init_hooks - _cbmem_init_hooks)

extern u8 _stack[];
extern u8 _estack[];
#define _stack_size (_estack - _stack)

extern u8 _pagetables[];
extern u8 _epagetables[];
#define _pagetables_size (_epagetables - _pagetables)

extern u8 _preram_cbfs_cache[];
extern u8 _epreram_cbfs_cache[];
#define _preram_cbfs_cache_size (_epreram_cbfs_cache - _preram_cbfs_cache)

extern u8 _postram_cbfs_cache[];
extern u8 _epostram_cbfs_cache[];
#define _postram_cbfs_cache_size (_epostram_cbfs_cache - _postram_cbfs_cache)

extern u8 _cbfs_cache[];
extern u8 _ecbfs_cache[];
#define _cbfs_cache_size (_ecbfs_cache - _cbfs_cache)

extern u8 _payload[];
extern u8 _epayload[];
#define _payload_size (_epayload - _payload)

/* "program" always refers to the current execution unit. */
extern u8 _program[];
extern u8 _eprogram[];
#define _program_size (_eprogram - _program)

/* _<stage>_size is always the maximum amount allocated in memlayout, whereas
 * _program_size gives the actual memory footprint *used* by current stage. */
extern u8 _bootblock[];
extern u8 _ebootblock[];
#define _bootblock_size (_ebootblock - _bootblock)

extern u8 _romstage[];
extern u8 _eromstage[];
#define _romstage_size (_eromstage - _romstage)

extern u8 _ramstage[];
extern u8 _eramstage[];
#define _ramstage_size (_eramstage - _ramstage)

extern u8 _verstage[];
extern u8 _everstage[];
#define _verstage_size (_everstage - _verstage)

/* Arch-specific, move to <arch/symbols.h> if they become too many. */

extern u8 _ttb[];
extern u8 _ettb[];
#define _ttb_size (_ettb - _ttb)

extern u8 _ttb_subtables[];
extern u8 _ettb_subtables[];
#define _ttb_subtables_size (_ettb_subtables - _ttb_subtables)

extern u8 _dma_coherent[];
extern u8 _edma_coherent[];
#define _dma_coherent_size (_edma_coherent - _dma_coherent)

extern u8 _soc_registers[];
extern u8 _esoc_registers[];
#define _soc_registers_size (_esoc_registers - _soc_registers)

extern u8 _framebuffer[];
extern u8 _eframebuffer[];
#define _framebuffer_size (_eframebuffer - _framebuffer)

extern u8 _pdpt[];
extern u8 _epdpt[];
#define _pdpt_size (_epdpt - _pdpt)

/* Put this into a .c file accessing a linker script region to mark that region
 * as "optional". If it is defined in memlayout.ld (or anywhere else), the
 * values from that definition will be used. If not, start, end and size will
 * all evaluate to 0. (We can't explicitly assign the symbols to 0 in the
 * assembly due to https://sourceware.org/bugzilla/show_bug.cgi?id=1038.) */
#define DECLARE_OPTIONAL_REGION(name) asm (".weak _" #name ", _e" #name)

#endif /* __SYMBOLS_H */
