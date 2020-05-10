/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMDBLOCKS_BIOSRAM_H__
#define __AMDBLOCKS_BIOSRAM_H__

#include <stdint.h>

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
