/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Google LLC
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

#ifndef __SOC_SYMBOLS_H__
#define __SOC_SYMBOLS_H__

extern unsigned char _bl31_sram[];
extern unsigned char _ebl31_sram[];
#define _bl31_sram_size (_ebl31_sram - _bl31_sram)

extern unsigned char _pmu_sram[];
extern unsigned char _epmu_sram[];
#define _pmu_sram_size (_epmu_sram - _pmu_sram)

#endif
