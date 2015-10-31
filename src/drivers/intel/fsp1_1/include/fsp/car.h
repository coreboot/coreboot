/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#ifndef FSP1_1_CAR_H
#define FSP1_1_CAR_H

#include <arch/cpu.h>
#include <fsp/api.h>
#include <stdint.h>

/* cache-as-ram support for FSP 1.1. */
struct cache_as_ram_params {
	uint64_t tsc;
	uint32_t bist;
	FSP_INFO_HEADER *fih;
	uintptr_t bootloader_car_start;
	uintptr_t bootloader_car_end;
};

/* Entry points from the cache-as-ram assembly code. */
asmlinkage void *cache_as_ram_main(struct cache_as_ram_params *car_params);
asmlinkage void after_cache_as_ram(void *chipset_context);
asmlinkage void *romstage_after_verstage(void);
/* Per stage calls from the above two functions. The void * return from
 * cache_as_ram_stage_main() is the stack pointer to use in ram after
 * exiting cache-as-ram mode. */
void *cache_as_ram_stage_main(FSP_INFO_HEADER *fih);
void after_cache_as_ram_stage(void);

/* Mainboard and SoC initialization prior to console. */
void car_mainboard_pre_console_init(void);
void car_soc_pre_console_init(void);
/* Mainboard and SoC initialization post console initialization. */
void car_mainboard_post_console_init(void);
void car_soc_post_console_init(void);

#endif
