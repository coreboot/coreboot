/*
 * This file is part of the coreboot project.
 *
 * (C) Copyright 2010,2011
 * NVIDIA Corporation <www.nvidia.com>
 *  Copyright (C) 2013 Google Inc.
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

#ifndef __NVIDIA_TEGRA124_DMA_H__
#define __NVIDIA_TEGRA124_DMA_H__

#include <inttypes.h>
#include <compiler.h>
#include <soc/addressmap.h>

/*
 * The DMA engine operates on 4 bytes at a time, so make sure any data
 * passed via DMA is aligned to avoid underrun/overrun.
 */
#define TEGRA_DMA_ALIGN_BYTES	4

/*
 * Note: Many APB DMA controller registers are laid out such that each
 * bit controls or represents the status for the corresponding channel.
 * So we will not bother to list each individual bit in this case.
 */
#define APB_COMMAND_GEN			(1 << 31)

#define APB_CNTRL_REG_COUNT_VALUE_MASK	0xffff
#define APB_CNTRL_REG_COUNT_VALUE_SHIFT	0

/*
 * Note: Many APB DMA controller registers are laid out such that each
 * bit controls or represents the status for the corresponding channel.
 * So we will not bother to list each individual bit in this case.
 */
#define APB_COMMAND_GEN			(1 << 31)

#define APB_CNTRL_REG_COUNT_VALUE_MASK	0xffff
#define APB_CNTRL_REG_COUNT_VALUE_SHIFT	0
struct apb_dma {
	u32 command;		/* 0x00 */
	u32 status;		/* 0x04 */
	u32 rsvd1[2];
	u32 cntrl_reg;		/* 0x10 */
	u32 irq_sta_cpu;	/* 0x14 */
	u32 irq_sta_cop;	/* 0x18 */
	u32 irq_mask;		/* 0x1c */
	u32 irq_mask_set;	/* 0x20 */
	u32 irq_mask_clr;	/* 0x24 */
	u32 trig_reg;		/* 0x28 */
	u32 channel_trig_reg;	/* 0x2c */
	u32 dma_status;		/* 0x30 */
	u32 channel_en_reg;	/* 0x34 */
	u32 security_reg;	/* 0x38 */
	u32 channel_swid;	/* 0x3c */
	u32 rsvd[1];
	u32 chan_wt_reg0;	/* 0x44 */
	u32 chan_wt_reg1;	/* 0x48 */
	u32 chan_wt_reg2;	/* 0x4c */
	u32 chan_wr_reg3;	/* 0x50 */
	u32 channel_swid1;	/* 0x54 */
} __packed;
check_member(apb_dma, channel_swid1, 0x54);

/*
 * Naming in the doc included a superfluous _CHANNEL_n_ for
 * each entry and was left out for the sake of conciseness.
 */
#define APB_CSR_ENB			(1 << 31)
#define APB_CSR_IE_EOC			(1 << 30)
#define APB_CSR_HOLD			(1 << 29)
#define APB_CSR_DIR			(1 << 28)
#define APB_CSR_ONCE			(1 << 27)
#define APB_CSR_FLOW			(1 << 21)
#define APB_CSR_REQ_SEL_MASK		0x1f
#define APB_CSR_REQ_SEL_SHIFT		16

enum apbdmachan_req_sel {
	APBDMA_SLAVE_CNTR_REQ	= 0,
	APBDMA_SLAVE_APBIF_CH0	= 1,
	APBDMA_SLAVE_APBIF_CH1	= 2,
	APBDMA_SLAVE_APBIF_CH2	= 3,
	APBDMA_SLAVE_APBIF_CH3	= 4,
	APBDMA_SLAVE_HSI	= 5,
	APBDMA_SLAVE_APBIF_CH4	= 6,
	APBDMA_SLAVE_APBIF_CH5	= 7,
	APBDMA_SLAVE_UART_A	= 8,
	APBDMA_SLAVE_UART_B	= 9,
	APBDMA_SLAVE_UART_C	= 10,
	APBDMA_SLAVE_DTV	= 11,
	APBDMA_SLAVE_APBIF_CH6	= 12,
	APBDMA_SLAVE_APBIF_CH7	= 13,
	APBDMA_SLAVE_APBIF_CH8	= 14,
	APBDMA_SLAVE_SL2B1	= 15,
	APBDMA_SLAVE_SL2B2	= 16,
	APBDMA_SLAVE_SL2B3	= 17,
	APBDMA_SLAVE_SL2B4	= 18,
	APBDMA_SLAVE_UART_D	= 19,
	APBDMA_SLAVE_UART_E	= 20,
	APBDMA_SLAVE_I2C	= 21,
	APBDMA_SLAVE_I2C2	= 22,
	APBDMA_SLAVE_I2C3	= 23,
	APBDMA_SLAVE_DVC_I2C	= 24,
	APBDMA_SLAVE_OWR	= 25,
	APBDMA_SLAVE_I2C4	= 26,
	APBDMA_SLAVE_SL2B5	= 27,
	APBDMA_SLAVE_SL2B6	= 28,
	APBDMA_SLAVE_APBIF_CH9	= 29,
	APBDMA_SLAVE_I2C6	= 30,
	APBDMA_SLAVE_NA31	= 31,
};

#define APB_STA_BSY			(1 << 31)
#define APB_STA_ISE_EOC			(1 << 30)
#define APB_STA_HALT			(1 << 29)
#define APB_STA_PING_PONG_STA		(1 << 28)
#define APB_STA_DMA_ACTIVITY		(1 << 27)
#define APB_STA_CHANNEL_PAUSE		(1 << 26)

#define APB_CSRE_CHANNEL_PAUSE		(1 << 31)
#define APB_CSRE_TRIG_SEL_MASK		0x3f
#define APB_CSRE_TRIG_SEL_SHIFT		14

#define AHB_PTR_MASK			(0x3fffffff)
#define AHB_PTR_SHIFT			2

#define AHB_SEQ_INTR_ENB		(1 << 31)
#define AHB_BUS_WIDTH_MASK		0x7
#define AHB_BUS_WIDTH_SHIFT		28
#define AHB_DATA_SWAP			(1 << 27)
#define AHB_BURST_MASK			0x7
#define AHB_BURST_SHIFT			24
#define AHB_SEQ_DBL_BUF			(1 << 19)
#define AHB_SEQ_WRAP_MASK		0x7
#define AHB_SEQ_WRAP_SHIFT		16

#define APB_PTR_MASK			0x3fffffff
#define APB_PTR_SHIFT			2

#define APB_BUS_WIDTH_MASK		0x7
#define APB_BUS_WIDTH_SHIFT		28
#define APB_DATA_SWAP			(1 << 27)
#define APB_ADDR_WRAP_MASK		0x7
#define APB_ADDR_WRAP_SHIFT		16

#define APB_WORD_TRANSFER_MASK		0x0fffffff
#define APB_WORD_TRANSFER_SHIFT		2

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
} __packed;
check_member(apb_dma_channel_regs, word_transfer, 0x24);

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
