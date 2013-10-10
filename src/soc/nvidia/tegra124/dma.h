/*
 * (C) Copyright 2010,2011
 * NVIDIA Corporation <www.nvidia.com>
 *  Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __NVIDIA_TEGRA124_DMA_H__
#define __NVIDIA_TEGRA124_DMA_H__

#include <inttypes.h>
#include <soc/addressmap.h>

/*
 * The DMA engine operates on 4 bytes at a time, so make sure any data
 * passed via DMA is aligned to avoid underrun/overrun.
 */
#define TEGRA_DMA_ALIGN_BYTES	4

struct apb_dma_channel_regs {
	u32 csr;		/* 0x00 */
	u32 sta;		/* 0x04 */
	u32 dma_byte_sta;	/* 0x08 */
	u32 csre;		/* 0x0c */
	u32 ahb_ptr;		/* 0x10 */
	u32 ahb_seq;		/* 0x14 */
	u32 apb_ptr;		/* 0x18 */
	u32 apb_seq;		/* 0x1c */
	u32 wcount;		/* 0x20 */
	u32 word_transfer;	/* 0x24 */
} __attribute__((packed));

struct apb_dma_channel {
	const int num;
	struct apb_dma_channel_regs *regs;

	/*
	 * Basic high-level semaphore that can be used to "claim"
	 * a DMA channel e.g. by SPI, I2C, or other peripheral driver.
	 */
	int in_use;
};

struct apb_dma_channel * const dma_claim(void);
void dma_release(struct apb_dma_channel * const channel);
int dma_start(struct apb_dma_channel * const channel);
int dma_stop(struct apb_dma_channel * const channel);
int dma_busy(struct apb_dma_channel * const channel);

#endif	/* __NVIDIA_TEGRA124_DMA_H__ */
