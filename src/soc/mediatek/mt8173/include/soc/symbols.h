/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#ifndef __SOC_MEDIATEK_MT8173_DRAM_DMA_H__
#define __SOC_MEDIATEK_MT8173_DRAM_DMA_H__

extern unsigned char _dram_dma[];
extern unsigned char _edram_dma[];
#define _dram_dma_size (_edram_dma - _dram_dma)

#endif
