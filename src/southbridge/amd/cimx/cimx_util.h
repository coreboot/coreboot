/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#ifndef CIMX_UTIL_H
#define CIMX_UTIL_H

#include <stdint.h>

/* FCH index/data registers */
#define BIOSRAM_INDEX	0xcd4
#define BIOSRAM_DATA	0xcd5
#define PM_INDEX		0xcd6
#define PM_DATA			0xcd7
#define PM2_INDEX		0xcd0
#define PM2_DATA		0xcd1
#define PCI_INTR_INDEX	0xc00
#define PCI_INTR_DATA	0xc01

void pm_iowrite(u8 reg, u8 value);
u8 pm_ioread(u8 reg);
void pm2_iowrite(u8 reg, u8 value);
u8 pm2_ioread(u8 reg);

#endif /* CIMX_UTIL_H */
