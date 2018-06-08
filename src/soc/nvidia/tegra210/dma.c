/*
 * This file is part of the coreboot project.
 *
 * (C) Copyright 2010,2011
 * NVIDIA Corporation <www.nvidia.com>
 * Copyright 2014 Google Inc.
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

#include <arch/io.h>
#include <console/console.h>
#include <inttypes.h>
#include <soc/addressmap.h>
#include <soc/dma.h>
#include <stddef.h>
#include <stdlib.h>

struct apb_dma * const apb_dma = (struct apb_dma *)TEGRA_APB_DMA_BASE;

#define APB_DMA_OFFSET(n) \
		(struct apb_dma_channel_regs *)(TEGRA_APB_DMA_BASE + n)
struct apb_dma_channel apb_dma_channels[] = {
	{ .num = 0, .regs = APB_DMA_OFFSET(0x1000) },
	{ .num = 1, .regs = APB_DMA_OFFSET(0x1040) },
	{ .num = 2, .regs = APB_DMA_OFFSET(0x1080) },
	{ .num = 3, .regs = APB_DMA_OFFSET(0x10c0) },
	{ .num = 4, .regs = APB_DMA_OFFSET(0x1100) },
	{ .num = 5, .regs = APB_DMA_OFFSET(0x1140) },
	{ .num = 6, .regs = APB_DMA_OFFSET(0x1180) },
	{ .num = 7, .regs = APB_DMA_OFFSET(0x11c0) },
	{ .num = 8, .regs = APB_DMA_OFFSET(0x1200) },
	{ .num = 9, .regs = APB_DMA_OFFSET(0x1240) },
	{ .num = 10, .regs = APB_DMA_OFFSET(0x1280) },
	{ .num = 11, .regs = APB_DMA_OFFSET(0x12c0) },
	{ .num = 12, .regs = APB_DMA_OFFSET(0x1300) },
	{ .num = 13, .regs = APB_DMA_OFFSET(0x1340) },
	{ .num = 14, .regs = APB_DMA_OFFSET(0x1380) },
	{ .num = 15, .regs = APB_DMA_OFFSET(0x13c0) },
	{ .num = 16, .regs = APB_DMA_OFFSET(0x1400) },
	{ .num = 17, .regs = APB_DMA_OFFSET(0x1440) },
	{ .num = 18, .regs = APB_DMA_OFFSET(0x1480) },
	{ .num = 19, .regs = APB_DMA_OFFSET(0x14c0) },
	{ .num = 20, .regs = APB_DMA_OFFSET(0x1500) },
	{ .num = 21, .regs = APB_DMA_OFFSET(0x1540) },
	{ .num = 22, .regs = APB_DMA_OFFSET(0x1580) },
	{ .num = 23, .regs = APB_DMA_OFFSET(0x15c0) },
	{ .num = 24, .regs = APB_DMA_OFFSET(0x1600) },
	{ .num = 25, .regs = APB_DMA_OFFSET(0x1640) },
	{ .num = 26, .regs = APB_DMA_OFFSET(0x1680) },
	{ .num = 27, .regs = APB_DMA_OFFSET(0x16c0) },
	{ .num = 28, .regs = APB_DMA_OFFSET(0x1700) },
	{ .num = 29, .regs = APB_DMA_OFFSET(0x1740) },
	{ .num = 30, .regs = APB_DMA_OFFSET(0x1780) },
	{ .num = 31, .regs = APB_DMA_OFFSET(0x17c0) },
};

int dma_busy(struct apb_dma_channel * const channel)
{
	/*
	 * In continuous mode, the BSY_n bit in APB_DMA_STATUS and
	 * BSY in APBDMACHAN_CHANNEL_n_STA_0 will remain set as '1' so long
	 * as the channel is enabled. So for this function we'll use the
	 * DMA_ACTIVITY bit in case of continuous mode.
	 *
	 * However, for ONCE mode, the BSY_n bit in APB_DMA_STATUS will be used
	 * to determine end of dma operation.
	 */
	uint32_t bit;

	if (read32(&channel->regs->csr) & APB_CSR_ONCE)
		/* Once mode */
		bit = APB_STA_BSY;
	else
		/* Continuous mode */
		bit = APB_STA_DMA_ACTIVITY;

	return read32(&channel->regs->sta) & bit ? 1 : 0;
}
/* claim a DMA channel */
struct apb_dma_channel * const dma_claim(void)
{
	int i;
	struct apb_dma_channel_regs *regs = NULL;

	/*
	 * Set global enable bit, otherwise register access to channel
	 * DMA registers will not be possible.
	 */
	setbits_le32(&apb_dma->command, APB_COMMAND_GEN);

	for (i = 0; i < ARRAY_SIZE(apb_dma_channels); i++) {
		regs = apb_dma_channels[i].regs;

		if (!apb_dma_channels[i].in_use) {
			u32 status = read32(&regs->sta);
			if (status & (1 << i)) {
				/* FIXME: should this be fatal? */
				printk(BIOS_DEBUG, "%s: DMA channel %d busy?\n",
						__func__, i);
			}
			break;
		}
	}

	if (i == ARRAY_SIZE(apb_dma_channels))
		return NULL;

	apb_dma_channels[i].in_use = 1;
	return &apb_dma_channels[i];
}

/* release a DMA channel */
void dma_release(struct apb_dma_channel * const channel)
{
	int i;

	/* FIXME: make this "thread" friendly */
	while (dma_busy(channel))
		;

	channel->in_use = 0;

	/* clear the global enable bit if no channels are in use */
	for (i = 0; i < ARRAY_SIZE(apb_dma_channels); i++) {
		if (apb_dma_channels[i].in_use)
			return;
	}

	clrbits_le32(&apb_dma->command, APB_COMMAND_GEN);
}

int dma_start(struct apb_dma_channel * const channel)
{
	struct apb_dma_channel_regs *regs = channel->regs;

	/* Set ENB bit for this channel */
	setbits_le32(&regs->csr, APB_CSR_ENB);

	return 0;
}

int dma_stop(struct apb_dma_channel * const channel)
{
	struct apb_dma_channel_regs *regs = channel->regs;

	/* Clear ENB bit for this channel */
	clrbits_le32(&regs->csr, APB_CSR_ENB);

	return 0;
}
