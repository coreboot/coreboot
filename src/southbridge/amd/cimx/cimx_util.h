/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef CIMX_UTIL_H
#define CIMX_UTIL_H

#include <stdint.h>

/* Power management index/data registers */
#define PM_INDEX	0xcd6
#define PM_DATA		0xcd7
#define PM2_INDEX	0xcd0
#define PM2_DATA	0xcd1

void pm_iowrite(u8 reg, u8 value);
u8 pm_ioread(u8 reg);
void pm2_iowrite(u8 reg, u8 value);
u8 pm2_ioread(u8 reg);

#endif /* CIMX_UTIL_H */
