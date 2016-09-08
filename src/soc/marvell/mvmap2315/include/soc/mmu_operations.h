/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#ifndef __SOC_MARVELL_MVMAP2315_MMU_OPERATIONS_H__
#define __SOC_MARVELL_MVMAP2315_MMU_OPERATIONS_H__

#include <stdint.h>

#define MVMAP2315_FLASH_SIZE		0x02000000
#define MVMAP2315_RAM_SIZE		0x80000000
#define MVMAP2315_DEVICE_SIZE		0x7E000000
#define MVMAP2315_LCM_SIZE		0x00020000

void mvmap2315_mmu_init(void);

#endif /*__SOC_MARVELL_MVMAP2315_MMU_OPERATIONS_H__*/
