/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018, The Linux Foundation.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_QUALCOMM_SDM845_SYMBOLS_H_
#define _SOC_QUALCOMM_SDM845_SYMBOLS_H_

#include <types.h>

extern u8 _ssram[];
extern u8 _essram[];
#define _ssram_size (_essram - _ssram)

extern u8 _bsram[];
extern u8 _ebsram[];
#define _bsram_size (_ebsram - _bsram)

extern u8 _dram_reserved[];
extern u8 _edram_reserved[];
#define _dram_reserved_size (_edram_reserved - _dram_reserved)

#endif // _SOC_QUALCOMM_SDM845_SYMBOLS_H_
