/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/i2c_simple.h>
#include <soc/i2c.h>
#include <soc/gpio.h>

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

void mtk_i2c_config_timing(struct mt_i2c_regs *regs, struct mtk_i2c *bus_ctrl)
{
	write32(&regs->clock_div, bus_ctrl->ac_timing.inter_clk_div);
	write32(&regs->timing, bus_ctrl->ac_timing.htiming);
	write32(&regs->ltiming, bus_ctrl->ac_timing.ltiming);
	write32(&regs->hs, bus_ctrl->ac_timing.hs);
	write32(&regs->ext_conf, bus_ctrl->ac_timing.ext);
}
