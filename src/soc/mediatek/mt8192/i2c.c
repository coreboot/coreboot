/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <gpio.h>
#include <soc/i2c.h>

#define I2C_FULL_DUTY 100
#define I2C_HALF_DUTY 50
#define I2C_ADJUSTED_DUTY 45
#define I2C_FS_START_CON 0x601
struct mtk_i2c mtk_i2c_bus_controller[] = {
	[0] = {
		.i2c_regs = (void *)(I2C_BASE + 0x250000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[1] = {
		.i2c_regs = (void *)(I2C_BASE + 0x70000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x80),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[2] = {
		.i2c_regs = (void *)(I2C_BASE + 0x71000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x100),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[3] = {
		.i2c_regs = (void *)(I2C_BASE),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x280),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[4] = {
		.i2c_regs = (void *)(I2C_BASE + 0x72000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x300),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[5] = {
		.i2c_regs = (void *)(I2C_BASE + 0x150000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x480),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[6] = {
		.i2c_regs = (void *)(I2C_BASE + 0x251000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x500),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[7] = {
		.i2c_regs = (void *)(I2C_BASE + 0x50000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x580),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[8] = {
		.i2c_regs = (void *)(I2C_BASE + 0x51000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x700),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
	[9] = {
		.i2c_regs = (void *)(I2C_BASE + 0x52000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x880),
		.mt_i2c_flag = I2C_APDMA_ASYNC,
	},
};

_Static_assert(ARRAY_SIZE(mtk_i2c_bus_controller) == I2C_BUS_NUMBER,
	       "Wrong size of mtk_i2c_bus_controller");

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
		PAD_FUNC(SDA5, SDA5),
		PAD_FUNC(SCL5, SCL5),
	},
	[6] = {
		PAD_FUNC(SDA6, SDA6),
		PAD_FUNC(SCL6, SCL6),
	},
	[7] = {
		PAD_FUNC(SDA7, SDA7),
		PAD_FUNC(SCL7, SCL7),
	},
	[8] = {
		PAD_FUNC(SDA8, SDA8),
		PAD_FUNC(SCL8, SCL8),
	},
	[9] = {
		PAD_FUNC(SDA9, SDA9),
		PAD_FUNC(SCL9, SCL9),
	},
};

static void mtk_i2c_set_gpio_pinmux(uint8_t bus)
{
	assert(bus < I2C_BUS_NUMBER);

	const struct pad_func *ptr = i2c_funcs[bus];
	for (size_t i = 0; i < 2; i++) {
		gpio_set_mode(ptr[i].gpio, ptr[i].func);
		gpio_set_pull(ptr[i].gpio, GPIO_PULL_ENABLE, GPIO_PULL_UP);
	}
}

static void mtk_i2c_speed_init_soc(uint8_t bus)
{
	uint8_t step_div;
	const uint8_t clock_div = 5;
	const uint8_t sample_div = 1;
	uint32_t i2c_freq;
	uint32_t tar_speed = 400;
	uint32_t tar_speed_high;
	uint32_t tar_speed_low;

	assert(bus < I2C_BUS_NUMBER);

	/* Adjust ratio of high/low level */
	tar_speed_high = tar_speed * I2C_HALF_DUTY / I2C_ADJUSTED_DUTY;

	/* Calculate i2c frequency */
	step_div = DIV_ROUND_UP(I2C_CLK_HZ,
				(tar_speed_high * KHz * sample_div * 2) * clock_div);
	i2c_freq = I2C_CLK_HZ / (step_div * sample_div * 2 * clock_div);
	assert(sample_div < 8 && step_div < 64 &&
	       i2c_freq <= tar_speed_high * KHz &&
	       i2c_freq >= (tar_speed_high - 20) * KHz);

	/* Init i2c bus timing register */
	write32(&mtk_i2c_bus_controller[bus].i2c_regs->timing,
		(sample_div - 1) << 8 | (step_div - 1));

	/* Adjust ratio of high/low level */
	tar_speed_low = tar_speed * I2C_HALF_DUTY /
			(I2C_FULL_DUTY - I2C_ADJUSTED_DUTY);

	/* Calculate i2c frequency */
	step_div = DIV_ROUND_UP(I2C_CLK_HZ,
				(tar_speed_low * KHz * sample_div * 2) * clock_div);
	i2c_freq = I2C_CLK_HZ / (step_div * sample_div * 2 * clock_div);
	assert(sample_div < 8 && step_div < 64 &&
	       i2c_freq <= tar_speed_low * KHz &&
	       i2c_freq >= (tar_speed_low - 20) * KHz);
	write32(&mtk_i2c_bus_controller[bus].i2c_regs->ltiming,
		(sample_div - 1) << 6 | (step_div - 1));

	/* Init i2c bus clock_div register */
	write32(&mtk_i2c_bus_controller[bus].i2c_regs->clock_div,
		clock_div - 1);

	/* Adjust tSU,STA/tHD,STA/tSU,STO */
	write32(&mtk_i2c_bus_controller[bus].i2c_regs->ext_conf, I2C_FS_START_CON);
}

void mtk_i2c_bus_init(uint8_t bus)
{
	mtk_i2c_speed_init_soc(bus);
	mtk_i2c_set_gpio_pinmux(bus);
}

void mtk_i2c_dump_more_info(struct mt_i2c_regs *regs)
{
	printk(BIOS_DEBUG, "LTIMING %x\nCLK_DIV %x\n",
	       read32(&regs->ltiming),
	       read32(&regs->clock_div));
}
