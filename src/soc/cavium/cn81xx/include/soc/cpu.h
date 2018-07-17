/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017-present Facebook, Inc.
 * Copyright 2003-2017    Cavium Inc.  <support@cavium.com>
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

#ifndef __SOC_CAVIUM_CN81XX_CPU_H__
#define __SOC_CAVIUM_CN81XX_CPU_H__

/**
 * Number of the Core on which the program is currently running.
 *
 * @return Number of cores
 */
size_t cpu_self_get_core_id(void);

/**
 * Return a mask representing this core in a 64bit bitmask
 *
 * @return The mask of active core.
 */
uint64_t cpu_self_get_core_mask(void);

/**
 * Return the mask of available cores.
 *
 * @return Mask of available cores
 */
uint64_t cpu_get_available_core_mask(void);

/**
 * Return the number of cores available in the chip.
 *
 * @return The number of available cores.
 */
size_t cpu_get_num_available_cores(void);

/**
 * Init secondary core and call the provided entry for given core.
 * A stack of size CONFIG_STACK_SIZE is set up for each core in REGION
 * stack_sec. The unique core id is passed to the entry point functions.
 *
 * @return zero on success
 */
size_t start_cpu(size_t cpu, void (*entry_64)(size_t core_id));

/**
 * Secondary ASM CPU entry point.
 * For internal use only.
 */
void secondary_init(void);

/**
 * Secondary CPU C entry point.
 * For internal use only.
 */
void secondary_cpu_init(size_t core_id);

/* Symbols in memlayout.ld */

extern u8 _stack_sec[];
extern u8 _estack_sec[];
#define _stack_sec_size (_estack_sec - _stack_sec)

#endif	/* __SOC_CAVIUM_CN81XX_CPU_H__ */
