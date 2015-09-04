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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
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

extern u8 _framebuffer[];
extern u8 _eframebuffer[];
#define _framebuffer_size (_eframebuffer - _framebuffer)

#endif /* __SYMBOLS_H */
