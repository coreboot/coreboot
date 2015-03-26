/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#ifndef _BROADWELL_IOBP_H_
#define _BROADWELL_IOBP_H_

u32 pch_iobp_read(u32 address);
void pch_iobp_write(u32 address, u32 data);
void pch_iobp_update(u32 address, u32 andvalue, u32 orvalue);
void pch_iobp_exec(u32 addr, u16 op_dcode, u8 route_id, u32 *data, u8 *resp);

#endif
