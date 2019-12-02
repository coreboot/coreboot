/*
 * This file is part of the coreboot project.
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

#ifndef __AMDBLOCKS_BIOSRAM_H__
#define __AMDBLOCKS_BIOSRAM_H__

#include <stdint.h>

/* BiosRam Ranges at 0xfed80500 or I/O 0xcd4/0xcd5 */
#define BIOSRAM_AP_ENTRY		0xe8 /* 8 bytes */
#define BIOSRAM_CBMEM_TOP		0xf0 /* 4 bytes */
#define BIOSRAM_UMA_SIZE		0xf4 /* 4 bytes */
#define BIOSRAM_UMA_BASE		0xf8 /* 8 bytes */

/* Returns the bootblock C entry point for APs */
void *get_ap_entry_ptr(void);
/* Used by BSP to store the bootblock entry point for APs */
void set_ap_entry_ptr(void *entry);
/* Saves the UMA size returned by AGESA */
void save_uma_size(uint32_t size);
/* Saves the UMA base address returned by AGESA */
void save_uma_base(uint64_t base);
/* Returns the saved UMA size */
uint32_t get_uma_size(void);
/* Returns the saved UMA base */
uint64_t get_uma_base(void);

#endif
