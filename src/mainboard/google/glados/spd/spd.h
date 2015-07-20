/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation
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

#ifndef MAINBOARD_SPD_H
#define MAINBOARD_SPD_H

#define SPD_LEN			256

#define SPD_DRAM_TYPE		2
#define  SPD_DRAM_DDR3		0x0b
#define  SPD_DRAM_LPDDR3	0xf1
#define SPD_DENSITY_BANKS	4
#define SPD_ADDRESSING		5
#define SPD_ORGANIZATION	7
#define SPD_BUS_DEV_WIDTH	8
#define SPD_PART_OFF		128
#define  SPD_PART_LEN		18
#define SPD_MANU_OFF		148

#endif
