/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <timer.h>
#include <symbols.h>
#include <device/mmio.h>
#include <soc/i2c.h>
#include <device/i2c_simple.h>

static inline void i2c_hw_reset(uint8_t bus)
{
	struct mt_i2c_regs *regs;
	struct mt_i2c_dma_regs *dma_regs;

	regs = mtk_i2c_bus_controller[bus].i2c_regs;
	dma_regs = mtk_i2c_bus_controller[bus].i2c_dma_regs;

	if (mtk_i2c_bus_controller[bus].mt_i2c_flag == I2C_APDMA_ASYNC) {
		write32(&dma_regs->dma_rst, I2C_DMA_WARM_RST);
		udelay(10);
		write32(&dma_regs->dma_rst, I2C_DMA_CLR_FLAG);
		udelay(10);
		write32(&dma_regs->dma_rst,
			I2C_DMA_HARD_RST | I2C_DMA_HANDSHAKE_RST);
		write32(&regs->softreset, I2C_SOFT_RST | I2C_HANDSHAKE_RST);
		udelay(10);
		write32(&dma_regs->dma_rst, I2C_DMA_CLR_FLAG);
		write32(&regs->softreset, I2C_CLR_FLAG);
	} else {
		write32(&regs->softreset, I2C_SOFT_RST);
		write32(&dma_regs->dma_rst, I2C_DMA_WARM_RST);
		udelay(50);
		write32(&dma_regs->dma_rst, I2C_DMA_HARD_RST);
		udelay(50);
		write32(&dma_regs->dma_rst, I2C_DMA_CLR_FLAG);
		udelay(50);
	}
}

static inline void mtk_i2c_dump_info(struct mt_i2c_regs *regs)
{
	printk(BIOS_ERR, "I2C register:\nSLAVE_ADDR %x\nINTR_MASK %x\n"
	       "INTR_STAT %x\nCONTROL %x\nTRANSFER_LEN %x\nTRANSAC_LEN %x\n"
	       "DELAY_LEN %x\nTIMING %x\nSTART %x\nFIFO_STAT %x\nIO_CONFIG %x\n"
	       "HS %x\nDEBUGSTAT %x\nEXT_CONF %x\n",
		read32(&regs->slave_addr),
		read32(&regs->intr_mask),
		read32(&regs->intr_stat),
		read32(&regs->control),
		read32(&regs->transfer_len),
		read32(&regs->transac_len),
		read32(&regs->delay_len),
		read32(&regs->timing),
		read32(&regs->start),
		read32(&regs->fifo_stat),
		read32(&regs->io_config),
		read32(&regs->hs),
		read32(&regs->debug_stat),
		read32(&regs->ext_conf));
}

static uint32_t mtk_i2c_transfer(uint8_t bus, struct i2c_msg *seg,
				 enum i2c_modes mode)
{
	uint32_t ret_code = I2C_OK;
	uint16_t status;
	uint16_t dma_sync = 0;
	uint32_t time_out_val = 0;
	uint8_t  addr;
	uint32_t write_len = 0;
	uint32_t read_len = 0;
	uint8_t *write_buffer = NULL;
	uint8_t *read_buffer = NULL;
	struct mt_i2c_regs *regs;
	struct mt_i2c_dma_regs *dma_regs;
	struct stopwatch sw;

	regs = mtk_i2c_bus_controller[bus].i2c_regs;
	dma_regs = mtk_i2c_bus_controller[bus].i2c_dma_regs;

	addr = seg[0].slave;

	if (mtk_i2c_bus_controller[bus].mt_i2c_flag == I2C_APDMA_ASYNC) {
		dma_sync = I2C_DMA_SKIP_CONFIG | I2C_DMA_ASYNC_MODE;
		if (mode == I2C_WRITE_READ_MODE)
			dma_sync |= I2C_DMA_DIR_CHANGE;
	}

	switch (mode) {
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

	switch (mode) {
	case I2C_WRITE_MODE:
		memcpy(_dma_coherent, write_buffer, write_len);

		/* control registers */
		write32(&regs->control, ASYNC_MODE | DMAACK_EN |
			ACK_ERR_DET_EN | DMA_EN | CLK_EXT |
			REPEATED_START_FLAG);

		/* Set transfer and transaction len */
		write32(&regs->transac_len, 1);
		write32(&regs->transfer_len, write_len);

		/* set i2c write slave address*/
		write32(&regs->slave_addr, addr << 1);

		/* Prepare buffer data to start transfer */
		write32(&dma_regs->dma_con, I2C_DMA_CON_TX | dma_sync);
		write32(&dma_regs->dma_tx_mem_addr, (uintptr_t)_dma_coherent);
		write32(&dma_regs->dma_tx_len, write_len);
		break;

	case I2C_READ_MODE:
		/* control registers */
		write32(&regs->control, ASYNC_MODE | DMAACK_EN |
			ACK_ERR_DET_EN | DMA_EN | CLK_EXT |
			REPEATED_START_FLAG);

		/* Set transfer and transaction len */
		write32(&regs->transac_len, 1);
		write32(&regs->transfer_len, read_len);

		/* set i2c read slave address*/
		write32(&regs->slave_addr, (addr << 1 | 0x1));

		/* Prepare buffer data to start transfer */
		write32(&dma_regs->dma_con, I2C_DMA_CON_RX | dma_sync);
		write32(&dma_regs->dma_rx_mem_addr, (uintptr_t)_dma_coherent);
		write32(&dma_regs->dma_rx_len, read_len);
		break;

	case I2C_WRITE_READ_MODE:
		memcpy(_dma_coherent, write_buffer, write_len);

		/* control registers */
		write32(&regs->control, ASYNC_MODE | DMAACK_EN |
			DIR_CHG | ACK_ERR_DET_EN | DMA_EN |
			CLK_EXT | REPEATED_START_FLAG);

		/* Set transfer and transaction len */
		write32(&regs->transfer_len, write_len);
		write32(&regs->transfer_aux_len, read_len);
		write32(&regs->transac_len, 2);

		/* set i2c write slave address*/
		write32(&regs->slave_addr, addr << 1);

		/* Prepare buffer data to start transfer */
		write32(&dma_regs->dma_con, I2C_DMA_CLR_FLAG | dma_sync);
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
			printk(BIOS_ERR, "[i2c%d] transfer NACK error\n", bus);
			mtk_i2c_dump_info(regs);
			break;
		} else if (status & I2C_ACKERR) {
			ret_code = I2C_TRANSFER_FAIL_ACKERR;
			printk(BIOS_ERR, "[i2c%d] transfer ACK error\n", bus);
			mtk_i2c_dump_info(regs);
			break;
		} else if (status & I2C_TRANSAC_COMP) {
			ret_code = I2C_OK;
			memcpy(read_buffer, _dma_coherent, read_len);
			break;
		}

		if (stopwatch_expired(&sw)) {
			ret_code = I2C_TRANSFER_FAIL_TIMEOUT;
			printk(BIOS_ERR, "[i2c%d] transfer timeout:%d\n", bus,
			       time_out_val);
			mtk_i2c_dump_info(regs);
			break;
		}
	}

	write32(&regs->intr_stat, I2C_TRANSAC_COMP | I2C_ACKERR |
		I2C_HS_NACKERR);

	/* clear bit mask */
	write32(&regs->intr_mask, I2C_HS_NACKERR | I2C_ACKERR |
		I2C_TRANSAC_COMP);

	/* reset the i2c controller for next i2c transfer. */
	i2c_hw_reset(bus);

	return ret_code;
}

static bool mtk_i2c_should_combine(struct i2c_msg *seg, int left_count)
{
	return (left_count >= 2 &&
	    !(seg[0].flags & I2C_M_RD) &&
	    (seg[1].flags & I2C_M_RD) &&
	    seg[0].slave == seg[1].slave);
}

int platform_i2c_transfer(unsigned int bus, struct i2c_msg *segments,
			  int seg_count)
{
	int ret = 0;
	int i;
	int mode;

	for (i = 0; i < seg_count; i++) {
		if (mtk_i2c_should_combine(&segments[i], seg_count - i)) {
			mode = I2C_WRITE_READ_MODE;
		} else {
			mode = (segments[i].flags & I2C_M_RD) ?
				I2C_READ_MODE : I2C_WRITE_MODE;
		}

		ret = mtk_i2c_transfer(bus, &segments[i], mode);

		if (ret)
			break;

		if (mode == I2C_WRITE_READ_MODE)
			i++;
	}

	return ret;
}
