/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <gpio.h>
#include <soc/i2c.h>

struct mtk_i2c mtk_i2c_bus_controller[] = {
	/* i2c0 setting */
	{
		.i2c_regs = (void *)(I2C_BASE + 0x2000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE),
	},

	/* i2c1 setting */
	{
		.i2c_regs = (void *)(I2C_BASE + 0xc000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x400),
	},

	/* i2c2 setting */
	{
		.i2c_regs = (void *)(I2C_BASE + 0x4000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x200),
	},

	/* i2c3 setting */
	{
		.i2c_regs = (void *)(I2C_BASE + 0xa000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x380),
	},

	/* i2c4 setting */
	{
		.i2c_regs = (void *)(I2C_BASE + 0x3000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x80),
	},

	/* i2c5 setting */
	{
		.i2c_regs = (void *)(I2C_BASE + 0x11000),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x480),
	},

	/* i2c6 setting */
	{
		.i2c_regs = (void *)(I2C_BASE),
		.i2c_dma_regs = (void *)(I2C_DMA_BASE + 0x580),
	},
};

_Static_assert(ARRAY_SIZE(mtk_i2c_bus_controller) == I2C_BUS_NUMBER,
	       "Wrong size of mtk_i2c_bus_controller");

static const struct pad_func i2c_funcs[I2C_BUS_NUMBER][2] = {
	{
		PAD_FUNC_UP(SDA0, SDA0),
		PAD_FUNC_UP(SCL0, SCL0),
	},
	{
		PAD_FUNC_UP(SDA1, SDA1),
		PAD_FUNC_UP(SCL1, SCL1),
	},
	{
		PAD_FUNC_UP(SDA2, SDA2),
		PAD_FUNC_UP(SCL2, SCL2),
	},
	{
		PAD_FUNC_UP(SDA3, SDA3),
		PAD_FUNC_UP(SCL3, SCL3),
	},
	{
		PAD_FUNC_UP(SDA4, SDA4),
		PAD_FUNC_UP(SCL4, SCL4),
	},
	{
		PAD_FUNC_UP(SDA5, SDA5),
		PAD_FUNC_UP(SCL5, SCL5),
	},
	{
		PAD_FUNC_UP(SDA6, SDA6),
		PAD_FUNC_UP(SCL6, SCL6),
	},
};

static void mtk_i2c_set_gpio_pinmux(uint8_t bus)
{
	assert(bus < I2C_BUS_NUMBER);

	const struct pad_func *ptr = i2c_funcs[bus];
	for (size_t i = 0; i < 2; i++) {
		gpio_set_mode(ptr[i].gpio, ptr[i].func);
		gpio_set_pull(ptr[i].gpio, GPIO_PULL_ENABLE, ptr[i].select);
	}
}

static void mtk_i2c_speed_init_soc(uint8_t bus)
{
	uint8_t step_div;
	const uint8_t clock_div = 5;
	const uint8_t sample_div = 1;
	uint32_t i2c_freq;

	assert(bus < ARRAY_SIZE(mtk_i2c_bus_controller));

	/* Calculate i2c frequency */
	step_div = DIV_ROUND_UP(I2C_CLK_HZ,
				(400 * KHz * sample_div * 2) * clock_div);
	i2c_freq = I2C_CLK_HZ / (step_div * sample_div * 2 * clock_div);
	assert(sample_div < 8 && step_div < 64 && i2c_freq <= 400 * KHz &&
	       i2c_freq >= 380 * KHz);

	/* Init i2c bus Timing register */
	write32(&mtk_i2c_bus_controller[bus].i2c_regs->timing,
		(sample_div - 1) << 8 | (step_div - 1));
	write32(&mtk_i2c_bus_controller[bus].i2c_regs->ltiming,
		(sample_div - 1) << 6 | (step_div - 1));

	/* Init i2c bus clock_div register */
	write32(&mtk_i2c_bus_controller[bus].i2c_regs->clock_div,
		clock_div - 1);
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
