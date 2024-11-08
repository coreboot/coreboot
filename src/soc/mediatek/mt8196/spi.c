/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8196 Functional Specification
 * Chapter number: 13.9
 */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/flash_controller_common.h>
#include <soc/spi.h>
#include <spi_flash.h>

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
		.cs_gpio = GPIO(EINT31),
	},
	{
		.regs = (void *)SPI3_BASE,
		.cs_gpio = GPIO(INT_SIM2),
	},
	{
		.regs = (void *)SPI4_BASE,
		.cs_gpio = GPIO(SPI_CSB_SEC),
	},
	{
		.regs = (void *)SPI5_BASE,
		.cs_gpio = GPIO(SPI5_CSB),
	},
	{
		.regs = (void *)SPI6_BASE,
		.cs_gpio = GPIO(I2SIN1_LRCK),
	},
	{
		.regs = (void *)SPI7_BASE,
		.cs_gpio = GPIO(EINT6),
	},
};

static const struct pad_func pad_funcs[SPI_BUS_NUMBER][4] = {
	{
		PAD_FUNC_DOWN(SPI0_MI, SPI0_MI),
		PAD_FUNC_GPIO(SPI0_CSB),
		PAD_FUNC_DOWN(SPI0_MO, SPI0_MO),
		PAD_FUNC_DOWN(SPI0_CLK, SPI0_CLK),
	},
	{
		PAD_FUNC_DOWN(SPI1_MI, SPI1_MI),
		PAD_FUNC_GPIO(SPI1_CSB),
		PAD_FUNC_DOWN(SPI1_MO, SPI1_MO),
		PAD_FUNC_DOWN(SPI1_CLK, SPI1_CLK),
	},
	{
		PAD_FUNC_DOWN(EINT28, SPI2_A_MI),
		PAD_FUNC_GPIO(EINT31),
		PAD_FUNC_DOWN(EINT29, SPI2_A_MO),
		PAD_FUNC_DOWN(EINT30, SPI2_A_CLK),
	},
	{
		PAD_FUNC_DOWN(EINT1, SPI3_A_MI),
		PAD_FUNC_GPIO(INT_SIM2),
		PAD_FUNC_DOWN(EINT0, SPI3_A_MO),
		PAD_FUNC_DOWN(INT_SIM1, SPI3_A_CLK),
	},
	{
		PAD_FUNC_DOWN(SPI_MI_SEC, SPI4_A_MI),
		PAD_FUNC_GPIO(SPI_CSB_SEC),
		PAD_FUNC_DOWN(SPI_MO_SEC, SPI4_A_MO),
		PAD_FUNC_DOWN(SPI_CLK_SEC, SPI4_A_CLK),
	},
	{
		PAD_FUNC_DOWN(SPI5_MI, SPI5_MI),
		PAD_FUNC_GPIO(SPI5_CSB),
		PAD_FUNC_DOWN(SPI5_MO, SPI5_MO),
		PAD_FUNC_DOWN(SPI5_CLK, SPI5_CLK),
	},
	{
		PAD_FUNC_DOWN(I2SOUT1_DO, SPI6_A_MI),
		PAD_FUNC_GPIO(I2SIN1_LRCK),
		PAD_FUNC_DOWN(I2SIN1_DI, SPI6_A_MO),
		PAD_FUNC_DOWN(I2SIN1_BCK, SPI6_A_CLK),
	},
	{
		PAD_FUNC_DOWN(EINT8, SPI7_A_MI),
		PAD_FUNC_GPIO(EINT6),
		PAD_FUNC_DOWN(EINT7, SPI7_A_MO),
		PAD_FUNC_DOWN(EINT5, SPI7_A_CLK),
	},
};

static const struct pad_func nor_pinmux[4] = {
	PAD_FUNC(SDA10, SF_CK, GPIO_PULL_DOWN),
	PAD_FUNC(SCL10, SF_CS, GPIO_PULL_UP),
	PAD_FUNC(PERIPHERAL_EN5, SF_D0, GPIO_PULL_DOWN),
	PAD_FUNC(PERIPHERAL_EN6, SF_D1, GPIO_PULL_DOWN),
};

void mtk_snfc_init(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(nor_pinmux); i++)
		mtk_snfc_init_pad_func(&nor_pinmux[i], GPIO_DRV_14_MA);
}

void mtk_spi_set_gpio_pinmux(unsigned int bus, enum spi_pad_mask pad_select)
{
	assert(bus < SPI_BUS_NUMBER);
	const struct pad_func *ptr;

	ptr = pad_funcs[bus];

	for (unsigned int i = 0; i < ARRAY_SIZE(pad_funcs[0]); i++)
		gpio_set_mode(ptr[i].gpio, ptr[i].func);
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
