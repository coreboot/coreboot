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

#include <stdint.h>
#include <drivers/intel/fsp1_0/fsp_util.h>

void main(FSP_INFO_HEADER *fsp_info_header);

#define NUM_ROMSTAGE_TS 4

void tco_disable(void);
void punit_init(void);
void early_mainboard_romstage_entry(void);
void late_mainboard_romstage_entry(void);
void get_func_disables(uint32_t *mask, uint32_t *mask2);
void byt_config_com1_and_enable(void);

#endif /* _BAYTRAIL_ROMSTAGE_H_ */
