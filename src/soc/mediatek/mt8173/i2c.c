/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#include <assert.h>
#include <delay.h>
#include <device/i2c.h>
#include <string.h>
#include <symbols.h>
#include <timer.h>
#include <arch/io.h>
#include <soc/addressmap.h>
#include <soc/i2c.h>
#include <soc/pll.h>

#define I2C_CLK_HZ (AXI_HZ / 16)

static struct mtk_i2c i2c[7] = {
	/* i2c0 setting */
	{
		.i2c_regs = (void *)I2C_BASE,
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x80),
	},

	/* i2c1 setting */
	{
		.i2c_regs = (void *)(I2C_BASE + 0x1000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x100),
	},

	/* i2c2 setting */
	{
		.i2c_regs = (void *)(I2C_BASE + 0x2000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x180),
	},

	/* i2c3 setting */
	{
		.i2c_regs = (void *)(I2C_BASE + 0x9000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x200),
	},

	/* i2c4 setting */
	{
		.i2c_regs = (void *)(I2C_BASE + 0xa000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x280),
	},

	/* i2c5 is reserved for internal use. */
	{
	},

	/* i2c6 setting */
	{
		.i2c_regs = (void *)(I2C_BASE + 0xc000),
		.i2c_dma_regs = (void *)I2C_DMA_BASE,
	}
};

#define I2CTAG                "[I2C][PL] "

#if IS_ENABLED(CONFIG_DEBUG_I2C)
#define I2CLOG(fmt, arg...)   printk(BIOS_INFO, I2CTAG fmt, ##arg)
#else
#define I2CLOG(fmt, arg...)
#endif /* CONFIG_DEBUG_I2C */

#define I2CERR(fmt, arg...)   printk(BIOS_ERR, I2CTAG fmt, ##arg)

static inline void i2c_dma_reset(struct mt8173_i2c_dma_regs *dma_regs)
{
	write32(&dma_regs->dma_rst, 0x1);
	udelay(50);
	write32(&dma_regs->dma_rst, 0x2);
	udelay(50);
	write32(&dma_regs->dma_rst, 0x0);
	udelay(50);
}

void mtk_i2c_bus_init(uint8_t bus)
{
	uint8_t sample_div;
	uint8_t step_div;
	uint32_t i2c_freq;

	assert(bus < ARRAY_SIZE(i2c));

	/* Calculate i2c frequency */
	sample_div = 1;
	step_div = div_round_up(I2C_CLK_HZ, (400 * KHz * sample_div * 2));
	i2c_freq = I2C_CLK_HZ / (step_div * sample_div * 2);
	assert(sample_div < 8 && step_div < 64 && i2c_freq < 400 * KHz &&
	       i2c_freq >= 380 * KHz);

	/* Init i2c bus Timing register */
	write32(&i2c[bus].i2c_regs->timing, (sample_div - 1) << 8 |
					    (step_div - 1));
}

static inline void mtk_i2c_dump_info(uint8_t bus)
{
	struct mt8173_i2c_regs *regs;

	regs = i2c[bus].i2c_regs;

	I2CLOG("I2C register:\nSLAVE_ADDR %x\nINTR_MASK %x\nINTR_STAT %x\n"
	       "CONTROL %x\nTRANSFER_LEN %x\nTRANSAC_LEN %x\nDELAY_LEN %x\n"
	       "TIMING %x\nSTART %x\nFIFO_STAT %x\nIO_CONFIG %x\nHS %x\n"
	       "DEBUGSTAT %x\nEXT_CONF %x\n",
		(read32(&regs->salve_addr)),
		(read32(&regs->intr_mask)),
		(read32(&regs->intr_stat)),
		(read32(&regs->control)),
		(read32(&regs->transfer_len)),
		(read32(&regs->transac_len)),
		(read32(&regs->delay_len)),
		(read32(&regs->timing)),
		(read32(&regs->start)),
		(read32(&regs->fifo_stat)),
		(read32(&regs->io_config)),
		(read32(&regs->hs)),
		(read32(&regs->debug_stat)),
		(read32(&regs->ext_conf)));

	I2CLOG("addr address %x\n", (uint32_t)regs);
}

static uint32_t mtk_i2c_transfer(uint8_t bus, struct i2c_seg *seg,
				 enum i2c_modes read)
{
	uint32_t ret_code = I2C_OK;
	uint16_t status;
	uint32_t time_out_val = 0;
	uint8_t  addr;
	uint32_t write_len = 0;
	uint32_t read_len = 0;
	uint8_t *write_buffer = NULL;
	uint8_t *read_buffer = NULL;
	struct mt8173_i2c_regs *regs;
	struct mt8173_i2c_dma_regs *dma_regs;
	struct stopwatch sw;

	regs = i2c[bus].i2c_regs;
	dma_regs = i2c[bus].i2c_dma_regs;

	addr = seg[0].chip;

	switch (read) {
	case I2C_WRITE_MODE:
		assert(seg[0].len > 0 && seg[0].len <= 255);
		write_len = seg[0].len;
		write_buffer = seg[0].buf;
		break;

	case I2C_READ_MODE:
		assert(seg[0].len > 0 && seg[0].len <= 255);
		read_len = seg[0].len;
		read_buffer = seg[0].buf;
		break;

	/* Must use special write-then-read mode for repeated starts. */
	case I2C_WRITE_READ_MODE:
		assert(seg[0].len > 0 && seg[0].len <= 255);
		assert(seg[1].len > 0 && seg[1].len <= 255);
		write_len = seg[0].len;
		read_len = seg[1].len;
		write_buffer = seg[0].buf;
		read_buffer = seg[1].buf;
		break;
	}

	/* Clear interrupt status */
	write32(&regs->intr_stat, I2C_TRANSAC_COMP | I2C_ACKERR |
		I2C_HS_NACKERR);

	write32(&regs->fifo_addr_clr, 0x1);

	/* Enable interrupt */
	write32(&regs->intr_mask, I2C_HS_NACKERR | I2C_ACKERR |
		I2C_TRANSAC_COMP);

	switch (read) {
	case I2C_WRITE_MODE:
		memcpy(_dma_coherent, write_buffer, write_len);

		/* control registers */
		write32(&regs->control, ACK_ERR_DET_EN | DMA_EN | CLK_EXT |
			REPEATED_START_FLAG);

		/* Set transfer and transaction len */
		write32(&regs->transac_len, 1);
		write32(&regs->transfer_len, write_len);

		/* set i2c write slave address*/
		write32(&regs->slave_addr, addr << 1);

		/* Prepare buffer data to start transfer */
		write32(&dma_regs->dma_con, I2C_DMA_CON_TX);
		write32(&dma_regs->dma_tx_mem_addr, (uintptr_t)_dma_coherent);
		write32(&dma_regs->dma_tx_len, write_len);
		break;

	case I2C_READ_MODE:
		/* control registers */
		write32(&regs->control, ACK_ERR_DET_EN | DMA_EN | CLK_EXT |
			REPEATED_START_FLAG);

		/* Set transfer and transaction len */
		write32(&regs->transac_len, 1);
		write32(&regs->transfer_len, read_len);

		/* set i2c read slave address*/
		write32(&regs->slave_addr, (addr << 1 | 0x1));

		/* Prepare buffer data to start transfer */
		write32(&dma_regs->dma_con, I2C_DMA_CON_RX);
		write32(&dma_regs->dma_rx_mem_addr, (uintptr_t)_dma_coherent);
		write32(&dma_regs->dma_rx_len, read_len);
		break;

	case I2C_WRITE_READ_MODE:
		memcpy(_dma_coherent, write_buffer, write_len);

		/* control registers */
		write32(&regs->control, DIR_CHG | ACK_ERR_DET_EN | DMA_EN |
			CLK_EXT | REPEATED_START_FLAG);

		/* Set transfer and transaction len */
		write32(&regs->transfer_len, write_len);
		write32(&regs->transfer_aux_len, read_len);
		write32(&regs->transac_len, 2);

		/* set i2c write slave address*/
		write32(&regs->slave_addr, addr << 1);

		/* Prepare buffer data to start transfer */
		write32(&dma_regs->dma_con, I2C_DMA_CLR_FLAG);
		write32(&dma_regs->dma_tx_mem_addr, (uintptr_t)_dma_coherent);
		write32(&dma_regs->dma_tx_len, write_len);
		write32(&dma_regs->dma_rx_mem_addr, (uintptr_t)_dma_coherent);
		write32(&dma_regs->dma_rx_len, read_len);
		break;
	}

	write32(&dma_regs->dma_int_flag, I2C_DMA_CLR_FLAG);
	write32(&dma_regs->dma_en, I2C_DMA_START_EN);

	/* start transfer transaction */
	write32(&regs->start, 0x1);

	stopwatch_init_msecs_expire(&sw, 100);

	/* polling mode : see if transaction complete */
	while (1) {
		status = read32(&regs->intr_stat);
		if (status & I2C_HS_NACKERR) {
			ret_code = I2C_TRANSFER_FAIL_HS_NACKERR;
			I2CERR("[i2c%d transfer] transaction NACK error\n",
			       bus);
			mtk_i2c_dump_info(bus);
			break;
		} else if (status & I2C_ACKERR) {
			ret_code = I2C_TRANSFER_FAIL_ACKERR;
			I2CERR("[i2c%d transfer] transaction ACK error\n", bus);
			mtk_i2c_dump_info(bus);
			break;
		} else if (status & I2C_TRANSAC_COMP) {
			ret_code = I2C_OK;
			memcpy(read_buffer, _dma_coherent, read_len);
			break;
		}

		if (stopwatch_expired(&sw)) {
			ret_code = I2C_TRANSFER_FAIL_TIMEOUT;
			I2CERR("[i2c%d transfer] transaction timeout:%d\n", bus,
			       time_out_val);
			mtk_i2c_dump_info(bus);
			break;
		}
	}

	write32(&regs->intr_stat, I2C_TRANSAC_COMP | I2C_ACKERR |
		I2C_HS_NACKERR);

	/* clear bit mask */
	write32(&regs->intr_mask, I2C_HS_NACKERR | I2C_ACKERR |
		I2C_TRANSAC_COMP);

	/* reset the i2c controller for next i2c transfer. */
	write32(&regs->softreset, 0x1);

	i2c_dma_reset(dma_regs);

	return ret_code;
}

static uint8_t mtk_i2c_should_combine(struct i2c_seg *seg, int left_count)
{
	if (left_count >= 2 && seg[0].read == 0 && seg[1].read == 1 &&
	    seg[0].chip == seg[1].chip)
		return 1;
	else
		return 0;
}

int platform_i2c_transfer(unsigned bus, struct i2c_seg *segments, int seg_count)
{
	int ret = 0;
	int i;
	int read;

	for (i = 0; i < seg_count; i++) {
		if (mtk_i2c_should_combine(&segments[i], seg_count - i))
			read = I2C_WRITE_READ_MODE;
		else
			read = segments[i].read;

		ret = mtk_i2c_transfer(bus, &segments[i], read);

		if (ret)
			break;

		if (read == I2C_WRITE_READ_MODE)
			i++;
	}

	return ret;
}
