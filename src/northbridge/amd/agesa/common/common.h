/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Sage Electronic Engineering, LLC
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

#ifndef _AGESA_COMMON_H_
#define _AGESA_COMMON_H_

#define SPD_CRC_HI  127
#define SPD_CRC_LO  126

struct  multi_spd_info {
	u8 offset;  // defines spd 0,1,...
	u8 size;    // defines spd size
};

AGESA_STATUS
common_ReadCbfsSpd (UINT32 Func, UINT32 Data, VOID *ConfigPtr);

#endif
