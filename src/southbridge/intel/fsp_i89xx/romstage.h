/*
 * This file is part of the coreboot project.
 *
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef _FSP_I89XX_ROMSTAGE_H_
#define _FSP_I89XX_ROMSTAGE_H_

#if !defined(__PRE_RAM__)
#error "Don't include romstage.h from a ramstage compilation unit!"
#endif

#include <stdint.h>
#include <arch/cpu.h>

void early_mainboard_romstage_entry(void);
void late_mainboard_romstage_entry(void);

void get_func_disables(uint32_t *fd_mask, uint32_t *fd2_mask);
uint16_t get_lpc_setting(void);

#endif /* _FSP_I89XX_ROMSTAGE_H_ */
