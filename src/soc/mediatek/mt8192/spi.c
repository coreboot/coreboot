/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <assert.h>
#include <soc/addressmap.h>
#include <soc/flash_controller_common.h>
#include <soc/gpio.h>
#include <soc/spi.h>

struct mtk_spi_bus spi_bus[SPI_BUS_NUMBER] = {
	{
		.regs = (void *)SPI0_BASE,
		.cs_gpio = GPIO(SPI0_CSB),
	},
	{
		.regs = (void *)SPI1_BASE,
		.cs_gpio = GPIO(SPI1_CSB),
	},
	{
		.regs = (void *)SPI2_BASE,
		.cs_gpio = GPIO(SCP_SPI2_CSB),
	},
	{
		.regs = (void *)SPI3_BASE,
		.cs_gpio = GPIO(CAM_RST1),
	},
	{
		.regs = (void *)SPI4_BASE,
		.cs_gpio = GPIO(EINT5),
	},
	{
		.regs = (void *)SPI5_BASE,
		.cs_gpio = GPIO(SPI5_CSB),
	},
	{
		.regs = (void *)SPI6_BASE,
		.cs_gpio = GPIO(EINT1),
	},
	{
		.regs = (void *)SPI7_BASE,
		.cs_gpio = GPIO(SDA0),
	}
};

struct pad_func {
	u8 pin_id;
	u8 func;
};

#define PAD_FUNC(name, func) {PAD_##name##_ID, PAD_##name##_FUNC_##func}
#define PAD_FUNC_GPIO(name) {PAD_##name##_ID, 0}

static const struct pad_func pad0_funcs[SPI_BUS_NUMBER][4] = {
	{
		PAD_FUNC(SPI0_MI, SPI0_A_MI),
		PAD_FUNC_GPIO(SPI0_CSB),
		PAD_FUNC(SPI0_MO, SPI0_A_MO),
		PAD_FUNC(SPI0_CLK, SPI0_A_CLK),
	},
	{
		PAD_FUNC(SPI1_MI, SPI1_A_MI),
		PAD_FUNC_GPIO(SPI1_CSB),
		PAD_FUNC(SPI1_MO, SPI1_A_MO),
		PAD_FUNC(SPI1_CLK, SPI1_A_CLK),
	},
	{
		PAD_FUNC(SCP_SPI2_MI, SPI2_MI),
		PAD_FUNC_GPIO(SCP_SPI2_CSB),
		PAD_FUNC(SCP_SPI2_MO, SPI2_MO),
		PAD_FUNC(SCP_SPI2_CK, SPI2_CLK),
	},
	{
		PAD_FUNC(CAM_RST2, SPI3_MI),
		PAD_FUNC_GPIO(CAM_RST1),
		PAD_FUNC(CAM_PDN0, SPI3_MO),
		PAD_FUNC(CAM_RST0, SPI3_CLK),
	},
	{
		PAD_FUNC(EINT6, SPI4_A_MI),
		PAD_FUNC_GPIO(EINT5),
		PAD_FUNC(EINT7, SPI4_A_MO),
		PAD_FUNC(EINT4, SPI4_A_CLK),
	},
	{
		PAD_FUNC(SPI5_MI, SPI5_A_MI),
		PAD_FUNC_GPIO(SPI5_CSB),
		PAD_FUNC(SPI5_MO, SPI5_A_MO),
		PAD_FUNC(SPI5_CLK, SPI5_A_CLK),
	},
	{
		PAD_FUNC(EINT2, SPI6_MI),
		PAD_FUNC_GPIO(EINT1),
		PAD_FUNC(EINT3, SPI6_MO),
		PAD_FUNC(EINT0, SPI6_CLK),
	},
	{
		PAD_FUNC(EINT16, SPI7_A_MI),
		PAD_FUNC_GPIO(SDA0),
		PAD_FUNC(EINT17, SPI7_A_MO),
		PAD_FUNC(SCL0, SPI7_A_CLK),
	}
};

void mtk_spi_set_gpio_pinmux(unsigned int bus, enum spi_pad_mask pad_select)
{
	assert(bus < SPI_BUS_NUMBER);
	assert(pad_select == SPI_PAD0_MASK);
	const struct pad_func *ptr = NULL;

	ptr = pad0_funcs[bus];
	for (int i = 0; i < 4; i++)
		gpio_set_mode((gpio_t){.id = ptr[i].pin_id}, ptr[i].func);
}

void mtk_spi_set_timing(struct mtk_spi_regs *regs, u32 sck_ticks, u32 cs_ticks,
			unsigned int tick_dly)
{
	write32(&regs->spi_cfg0_reg,
		((cs_ticks - 1) << SPI_CFG0_CS_HOLD_SHIFT) |
		((cs_ticks - 1) << SPI_CFG0_CS_SETUP_SHIFT));

	write32(&regs->spi_cfg2_reg,
		((sck_ticks - 1) << SPI_CFG2_SCK_HIGH_SHIFT) |
		((sck_ticks - 1) << SPI_CFG2_SCK_LOW_SHIFT));

	clrsetbits32(&regs->spi_cfg1_reg, SPI_CFG1_TICK_DLY_MASK |
		     SPI_CFG1_CS_IDLE_MASK,
		     (tick_dly << SPI_CFG1_TICK_DLY_SHIFT) |
		     ((cs_ticks - 1) << SPI_CFG1_CS_IDLE_SHIFT));
}

static const struct spi_ctrlr spi_flash_ctrlr = {
	.max_xfer_size = 65535,
	.flash_probe = mtk_spi_flash_probe,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = 0,
		.bus_end = SPI_BUS_NUMBER - 1,
	},
	{
		.ctrlr = &spi_flash_ctrlr,
		.bus_start = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS,
		.bus_end = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
