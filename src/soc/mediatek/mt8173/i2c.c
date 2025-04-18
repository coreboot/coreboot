/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/i2c.h>

struct mtk_i2c mtk_i2c_bus_controller[7] = {
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

_Static_assert(ARRAY_SIZE(mtk_i2c_bus_controller) == I2C_BUS_NUMBER,
	       "Wrong size of mtk_i2c_bus_controller");

#define I2CTAG                "[I2C][PL] "

#if CONFIG(DEBUG_I2C)
#define I2CLOG(fmt, arg...)   printk(BIOS_INFO, I2CTAG fmt, ##arg)
#else
#define I2CLOG(fmt, arg...)
#endif /* CONFIG_DEBUG_I2C */

void mtk_i2c_bus_init(uint8_t bus, uint32_t speed)
{
	uint8_t step_div;
	uint32_t i2c_freq;
	const uint8_t sample_div = 1;

	assert(bus < ARRAY_SIZE(mtk_i2c_bus_controller));
	assert(speed == I2C_SPEED_FAST);

	/* Calculate i2c frequency */
	step_div = DIV_ROUND_UP(I2C_CLK_HZ, (400 * KHz * sample_div * 2));
	i2c_freq = I2C_CLK_HZ / (step_div * sample_div * 2);
	assert(sample_div < 8 && step_div < 64 && i2c_freq < 400 * KHz &&
	       i2c_freq >= 380 * KHz);

	/* Init i2c bus Timing register */
	write32(&mtk_i2c_bus_controller[bus].i2c_regs->timing,
		(sample_div - 1) << 8 | (step_div - 1));
}
