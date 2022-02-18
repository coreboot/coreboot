/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <timer.h>
#include <symbols.h>
#include <device/mmio.h>
#include <soc/i2c.h>
#include <soc/i2c_common.h>
#include <device/i2c_simple.h>

const struct i2c_spec_values standard_mode_spec = {
	.min_low_ns = 4700 + I2C_STANDARD_MODE_BUFFER,
	.min_su_sta_ns = 4700 + I2C_STANDARD_MODE_BUFFER,
	.max_hd_dat_ns = 3450 - I2C_STANDARD_MODE_BUFFER,
	.min_su_dat_ns = 250 + I2C_STANDARD_MODE_BUFFER,
};

const struct i2c_spec_values fast_mode_spec = {
	.min_low_ns = 1300 + I2C_FAST_MODE_BUFFER,
	.min_su_sta_ns = 600 + I2C_FAST_MODE_BUFFER,
	.max_hd_dat_ns = 900 - I2C_FAST_MODE_BUFFER,
	.min_su_dat_ns = 100 + I2C_FAST_MODE_BUFFER,
};

const struct i2c_spec_values fast_mode_plus_spec = {
	.min_low_ns = 500 + I2C_FAST_MODE_PLUS_BUFFER,
	.min_su_sta_ns = 260 + I2C_FAST_MODE_PLUS_BUFFER,
	.max_hd_dat_ns = 400 - I2C_FAST_MODE_PLUS_BUFFER,
	.min_su_dat_ns = 50 + I2C_FAST_MODE_PLUS_BUFFER,
};

__weak void mtk_i2c_dump_more_info(struct mt_i2c_regs *regs)
{
	/* do nothing */
}

__weak void mtk_i2c_config_timing(struct mt_i2c_regs *regs, struct mtk_i2c *bus_ctrl)
{
	/* do nothing */
}

const struct i2c_spec_values *mtk_i2c_get_spec(uint32_t speed)
{
	if (speed <= I2C_SPEED_STANDARD)
		return &standard_mode_spec;
	else if (speed <= I2C_SPEED_FAST)
		return &fast_mode_spec;
	else
		return &fast_mode_plus_spec;
}

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
	printk(BIOS_DEBUG, "I2C register:\nSLAVE_ADDR %x\nINTR_MASK %x\n"
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

	mtk_i2c_dump_more_info(regs);
}

static int mtk_i2c_transfer(uint8_t bus, struct i2c_msg *seg,
			    enum i2c_modes mode)
{
	int ret = I2C_OK;
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

	stopwatch_init_usecs_expire(&sw, CONFIG_I2C_TRANSFER_TIMEOUT_US);

	/* polling mode : see if transaction complete */
	while (1) {
		status = read32(&regs->intr_stat);
		if (status & I2C_HS_NACKERR) {
			ret = I2C_TRANSFER_FAIL_HS_NACKERR;
			printk(BIOS_ERR, "[i2c%d] transfer NACK error\n", bus);
			mtk_i2c_dump_info(regs);
			break;
		} else if (status & I2C_ACKERR) {
			ret = I2C_TRANSFER_FAIL_ACKERR;
			printk(BIOS_ERR, "[i2c%d] transfer ACK error\n", bus);
			mtk_i2c_dump_info(regs);
			break;
		} else if (status & I2C_TRANSAC_COMP) {
			ret = I2C_OK;
			memcpy(read_buffer, _dma_coherent, read_len);
			break;
		}

		if (stopwatch_expired(&sw)) {
			ret = I2C_TRANSFER_FAIL_TIMEOUT;
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

	return ret;
}

static bool mtk_i2c_should_combine(struct i2c_msg *seg, int left_count)
{
	return (left_count >= 2 &&
	    !(seg[0].flags & I2C_M_RD) &&
	    (seg[1].flags & I2C_M_RD) &&
	    seg[0].slave == seg[1].slave);
}

static int mtk_i2c_max_step_cnt(uint32_t target_speed)
{
	if (target_speed > I2C_SPEED_FAST_PLUS)
		return MAX_HS_STEP_CNT_DIV;
	else
		return MAX_STEP_CNT_DIV;
}

int platform_i2c_transfer(unsigned int bus, struct i2c_msg *segments,
			  int seg_count)
{
	int ret;
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
		if (ret < 0)
			return ret;

		if (mode == I2C_WRITE_READ_MODE)
			i++;
	}

	return 0;
}

/*
 * Check and calculate i2c ac-timing.
 *
 * Hardware design:
 * sample_ns = (1000000000 * (sample_cnt + 1)) / clk_src
 * xxx_cnt_div =  spec->min_xxx_ns / sample_ns
 *
 * The calculation of sample_ns is rounded down;
 * otherwise xxx_cnt_div would be greater than the smallest spec.
 * The sda_timing is chosen as the middle value between
 * the largest and smallest.
 */
int mtk_i2c_check_ac_timing(uint8_t bus, uint32_t clk_src,
			    uint32_t check_speed,
			    uint32_t step_cnt,
			    uint32_t sample_cnt)
{
	const struct i2c_spec_values *spec;
	uint32_t su_sta_cnt, low_cnt, high_cnt, max_step_cnt;
	uint32_t sda_max, sda_min, clk_ns, max_sta_cnt = 0x100;
	uint32_t sample_ns = ((uint64_t)NSECS_PER_SEC * (sample_cnt + 1)) / clk_src;
	struct mtk_i2c_ac_timing *ac_timing;

	spec = mtk_i2c_get_spec(check_speed);

	clk_ns = NSECS_PER_SEC / clk_src;

	su_sta_cnt = DIV_ROUND_UP(spec->min_su_sta_ns, clk_ns);
	if (su_sta_cnt > max_sta_cnt)
		return -1;

	low_cnt = DIV_ROUND_UP(spec->min_low_ns, sample_ns);
	max_step_cnt = mtk_i2c_max_step_cnt(check_speed);
	if (2 * step_cnt > low_cnt && low_cnt < max_step_cnt) {
		if (low_cnt > step_cnt) {
			high_cnt = 2 * step_cnt - low_cnt;
		} else {
			high_cnt = step_cnt;
			low_cnt = step_cnt;
		}
	} else {
		return -2;
	}

	sda_max = spec->max_hd_dat_ns / sample_ns;
	if (sda_max > low_cnt)
		sda_max = 0;

	sda_min = DIV_ROUND_UP(spec->min_su_dat_ns, sample_ns);
	if (sda_min < low_cnt)
		sda_min = 0;

	if (sda_min > sda_max)
		return -3;

	ac_timing = &mtk_i2c_bus_controller[bus].ac_timing;
	if (check_speed > I2C_SPEED_FAST_PLUS) {
		ac_timing->hs = I2C_TIME_DEFAULT_VALUE | (sample_cnt << 12) | (high_cnt << 8);
		ac_timing->ltiming &= ~GENMASK(15, 9);
		ac_timing->ltiming |= (sample_cnt << 12) | (low_cnt << 9);
		ac_timing->ext &= ~GENMASK(7, 1);
		ac_timing->ext |= (su_sta_cnt << 1) | (1 << 0);
	} else {
		ac_timing->htiming = (sample_cnt << 8) | (high_cnt);
		ac_timing->ltiming = (sample_cnt << 6) | (low_cnt);
		ac_timing->ext = (su_sta_cnt << 8) | (1 << 0);
	}

	return 0;
}

/*
 * Calculate i2c port speed.
 *
 * Hardware design:
 * i2c_bus_freq = parent_clk / (clock_div * 2 * sample_cnt * step_cnt)
 * clock_div: fixed in hardware, but may be various in different SoCs
 *
 * To calculate sample_cnt and step_cnt, we pick the highest bus frequency
 * that is still no larger than i2c->speed_hz.
 */
int mtk_i2c_calculate_speed(uint8_t bus, uint32_t clk_src,
			    uint32_t target_speed,
			    uint32_t *timing_step_cnt,
			    uint32_t *timing_sample_cnt)
{
	uint32_t step_cnt;
	uint32_t sample_cnt;
	uint32_t max_step_cnt;
	uint32_t base_sample_cnt = MAX_SAMPLE_CNT_DIV;
	uint32_t base_step_cnt;
	uint32_t opt_div;
	uint32_t best_mul;
	uint32_t cnt_mul;
	uint32_t clk_div = mtk_i2c_bus_controller[bus].ac_timing.inter_clk_div;
	int32_t clock_div_constraint = 0;
	int success = 0;

	if (target_speed > I2C_SPEED_HIGH)
		target_speed = I2C_SPEED_HIGH;

	max_step_cnt = mtk_i2c_max_step_cnt(target_speed);
	base_step_cnt = max_step_cnt;

	/* Find the best combination */
	opt_div = DIV_ROUND_UP(clk_src >> 1, target_speed);
	best_mul = MAX_SAMPLE_CNT_DIV * max_step_cnt;

	/* Search for the best pair (sample_cnt, step_cnt) with
	 * 0 < sample_cnt < MAX_SAMPLE_CNT_DIV
	 * 0 < step_cnt < max_step_cnt
	 * sample_cnt * step_cnt >= opt_div
	 * optimizing for sample_cnt * step_cnt being minimal
	 */
	for (sample_cnt = 1; sample_cnt <= MAX_SAMPLE_CNT_DIV; sample_cnt++) {
		if (sample_cnt == 1) {
			if (clk_div != 0)
				clock_div_constraint = 1;
			else
				clock_div_constraint = 0;
		} else {
			if (clk_div > 1)
				clock_div_constraint = 1;
			else if (clk_div == 0)
				clock_div_constraint = -1;
			else
				clock_div_constraint = 0;
		}

		step_cnt = DIV_ROUND_UP(opt_div + clock_div_constraint, sample_cnt);
		if (step_cnt > max_step_cnt)
			continue;

		cnt_mul = step_cnt * sample_cnt;
		if (cnt_mul >= best_mul)
			continue;

		if (mtk_i2c_check_ac_timing(bus, clk_src,
					    target_speed, step_cnt - 1,
					    sample_cnt - 1))
			continue;

		success = 1;
		best_mul = cnt_mul;
		base_sample_cnt = sample_cnt;
		base_step_cnt = step_cnt;
		if (best_mul == opt_div + clock_div_constraint)
			break;

	}

	if (!success)
		return -1;

	sample_cnt = base_sample_cnt;
	step_cnt = base_step_cnt;

	if (clk_src / (2 * (sample_cnt * step_cnt - clock_div_constraint)) >
	    target_speed)
		return -1;

	*timing_step_cnt = step_cnt - 1;
	*timing_sample_cnt = sample_cnt - 1;

	return 0;
}

void mtk_i2c_speed_init(uint8_t bus, uint32_t speed)
{
	uint32_t max_clk_div = MAX_CLOCK_DIV;
	uint32_t clk_src, clk_div, step_cnt, sample_cnt;
	uint32_t l_step_cnt, l_sample_cnt;
	struct mtk_i2c *bus_ctrl;

	if (bus >= I2C_BUS_NUMBER) {
		printk(BIOS_ERR, "%s, error bus num:%d\n", __func__, bus);
		return;
	}

	bus_ctrl = &mtk_i2c_bus_controller[bus];

	for (clk_div = 1; clk_div <= max_clk_div; clk_div++) {
		clk_src = I2C_CLK_HZ / clk_div;
		bus_ctrl->ac_timing.inter_clk_div = clk_div - 1;

		if (speed > I2C_SPEED_FAST_PLUS) {
			/* Set master code speed register */
			if (mtk_i2c_calculate_speed(bus, clk_src, I2C_SPEED_FAST,
						    &l_step_cnt, &l_sample_cnt))
				continue;

			/* Set the high speed mode register */
			if (mtk_i2c_calculate_speed(bus, clk_src, speed,
						    &step_cnt, &sample_cnt))
				continue;

			bus_ctrl->ac_timing.inter_clk_div = (clk_div - 1) << 8 | (clk_div - 1);
		} else {
			if (mtk_i2c_calculate_speed(bus, clk_src, speed,
						    &l_step_cnt, &l_sample_cnt))
				continue;

			/* Disable the high speed transaction */
			bus_ctrl->ac_timing.hs = I2C_TIME_CLR_VALUE;
		}

		break;
	}

	if (clk_div > max_clk_div) {
		printk(BIOS_ERR, "%s, cannot support %d hz on i2c-%d\n", __func__, speed, bus);
		return;
	}

	/* Init i2c bus timing register. */
	mtk_i2c_config_timing(bus_ctrl->i2c_regs, bus_ctrl);
}
