/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/i2c_simple.h>
#include <soc/pll.h>
#include <soc/i2c.h>
#include <soc/gpio.h>
#include <timer.h>

#define I2C_CLK_HZ (UNIVPLL_HZ / 20)

struct mtk_i2c mtk_i2c_bus_controller[] = {
	[0] = {
		.i2c_regs = (void *)(I2C_BASE),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[1] = {
		.i2c_regs = (void *)(I2C_BASE + 0x1000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x180),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[2] = {
		.i2c_regs = (void *)(I2C_BASE + 0x2000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x300),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[3] = {
		.i2c_regs = (void *)(I2C_BASE + 0x3000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x400),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[4] = {
		.i2c_regs = (void *)(I2C_BASE + 0x4000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x480),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[5] = {
		.i2c_regs = (void *)(I2C_BASE - 0x100000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x500),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[6] = {
		.i2c_regs = (void *)(I2C_BASE - 0xFF000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x580),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[7] = {
		.i2c_regs = (void *)(I2C_BASE - 0xFE000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x600),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
};

#define I2C_BUS_NUMBER ARRAY_SIZE(mtk_i2c_bus_controller)

struct pad_func {
	gpio_t gpio;
	u8 func;
};

#define PAD_FUNC(name, func) {GPIO(name), PAD_##name##_FUNC_##func}

static const struct pad_func i2c_funcs[I2C_BUS_NUMBER][2] = {
	[0] = {
		PAD_FUNC(SDA0, SDA0),
		PAD_FUNC(SCL0, SCL0),
	},
	[1] = {
		PAD_FUNC(SDA1, SDA1),
		PAD_FUNC(SCL1, SCL1),
	},
	[2] = {
		PAD_FUNC(SDA2, SDA2),
		PAD_FUNC(SCL2, SCL2),
	},
	[3] = {
		PAD_FUNC(SDA3, SDA3),
		PAD_FUNC(SCL3, SCL3),
	},
	[4] = {
		PAD_FUNC(SDA4, SDA4),
		PAD_FUNC(SCL4, SCL4),
	},
	[5] = {
		PAD_FUNC(HDMIRX_SCL, SCL5),
		PAD_FUNC(HDMIRX_SDA, SDA5),
	},
	[6] = {
		PAD_FUNC(HDMITX_SCL, SCL6),
		PAD_FUNC(HDMITX_SDA, SDA6),
	},
	[7] = {
		PAD_FUNC(HDMIRX_HTPLG, SCL7),
		PAD_FUNC(HDMIRX_PWR5V, SDA7),
	},

};

static void mtk_i2c_set_gpio_pinmux(uint8_t bus)
{
	assert(bus < I2C_BUS_NUMBER);

	const struct pad_func *ptr = i2c_funcs[bus];
	for (size_t i = 0; i < 2; i++) {
		gpio_set_mode(ptr[i].gpio, ptr[i].func);
		if (bus <= I2C4)
			gpio_set_pull(ptr[i].gpio, GPIO_PULL_ENABLE, GPIO_PULL_UP);
	}
}

static int mtk_i2c_max_step_cnt(uint32_t target_speed)
{
	if (target_speed > I2C_SPEED_FAST_PLUS)
		return MAX_HS_STEP_CNT_DIV;
	else
		return MAX_STEP_CNT_DIV;
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
static int mtk_i2c_check_ac_timing(uint8_t bus, uint32_t clk_src,
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
static int mtk_i2c_calculate_speed(uint8_t bus, uint32_t clk_src,
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

static void mtk_i2c_speed_init(uint8_t bus, uint32_t speed)
{
	uint32_t max_clk_div = MAX_CLOCK_DIV;
	uint32_t clk_src, clk_div, step_cnt, sample_cnt;
	uint32_t l_step_cnt, l_sample_cnt;
	uint32_t timing_reg_value, ltiming_reg_value;
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

			timing_reg_value = (l_sample_cnt << 8) | l_step_cnt;

			/* Set the high speed mode register */
			if (mtk_i2c_calculate_speed(bus, clk_src, speed,
						    &step_cnt, &sample_cnt))
				continue;

			ltiming_reg_value = (l_sample_cnt << 6) | l_step_cnt |
					    (sample_cnt << 12) | (step_cnt << 9);
			bus_ctrl->ac_timing.inter_clk_div = (clk_div - 1) << 8 | (clk_div - 1);
		} else {
			if (mtk_i2c_calculate_speed(bus, clk_src, speed,
						    &l_step_cnt, &l_sample_cnt))
				continue;

			timing_reg_value = (l_sample_cnt << 8) | l_step_cnt;

			/* Disable the high speed transaction */
			bus_ctrl->ac_timing.hs = I2C_TIME_CLR_VALUE;

			ltiming_reg_value = (l_sample_cnt << 6) | l_step_cnt;
		}

		break;
	}

	if (clk_div > max_clk_div) {
		printk(BIOS_ERR, "%s, cannot support %d hz on i2c-%d\n", __func__, speed, bus);
		return;
	}

	/* Init i2c bus timing register */
	write32(&bus_ctrl->i2c_regs->clock_div, bus_ctrl->ac_timing.inter_clk_div);
	write32(&bus_ctrl->i2c_regs->timing, bus_ctrl->ac_timing.htiming);
	write32(&bus_ctrl->i2c_regs->ltiming, bus_ctrl->ac_timing.ltiming);
	write32(&bus_ctrl->i2c_regs->hs, bus_ctrl->ac_timing.hs);
	write32(&bus_ctrl->i2c_regs->ext_conf, bus_ctrl->ac_timing.ext);
}

void mtk_i2c_bus_init(uint8_t bus, uint32_t speed)
{
	mtk_i2c_speed_init(bus, speed);
	mtk_i2c_set_gpio_pinmux(bus);
}

void mtk_i2c_dump_more_info(struct mt_i2c_regs *regs)
{
	printk(BIOS_DEBUG, "LTIMING %x\nCLK_DIV %x\n",
	       read32(&regs->ltiming),
	       read32(&regs->clock_div));
}
