/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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

#ifndef _BAYTRAIL_ROMSTAGE_H_
#define _BAYTRAIL_ROMSTAGE_H_

#if !defined(__PRE_RAM__)
#error "Don't include romstage.h from a ramstage compilation unit!"
#endif

void report_platform_info(void);

#include <stdint.h>
#include <arch/cpu.h>
#include <drivers/intel/fsp1_0/fsp_util.h>

void main(FSP_INFO_HEADER *fsp_info_header);
void romstage_main_continue(EFI_STATUS status, void *hob_list_ptr);
uint32_t chipset_prev_sleep_state(uint32_t clear);

#define NUM_ROMSTAGE_TS 4

void tco_disable(void);
void punit_init(void);
void set_max_freq(void);
void early_mainboard_romstage_entry(void);
void late_mainboard_romstage_entry(void);
void get_func_disables(uint32_t *mask, uint32_t *mask2);

#if IS_ENABLED(CONFIG_ENABLE_BUILTIN_COM1)
void byt_config_com1_and_enable(void);
#else
static inline void byt_config_com1_and_enable(void) { }
#endif

#endif /* _BAYTRAIL_ROMSTAGE_H_ */
