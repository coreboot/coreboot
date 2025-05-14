/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 9.17
 */

#include <gpio.h>
#include <soc/flash_controller_common.h>
#include <soc/spi.h>
#include <spi_flash.h>

struct mtk_spi_bus spi_bus[SPI_BUS_NUMBER] = {
	{
		.regs = (void *)SPI0_BASE,
		.cs_gpio = GPIO(SPIM0_CSB),
	},
	{
		.regs = (void *)SPI1_BASE,
		.cs_gpio = GPIO(SPIM1_CSB),
	},
	{
		.regs = (void *)SPI2_BASE,
		.cs_gpio = GPIO(SPIM2_CSB),
	},
	{
		.regs = (void *)SPI3_BASE,
		.cs_gpio = GPIO(GPIO00),
	},
	{
		.regs = (void *)SPI4_BASE,
		.cs_gpio = GPIO(GPIO04),
	},
	{
		.regs = (void *)SPI5_BASE,
		.cs_gpio = GPIO(GPIO08),
	},
};

static const struct pad_func pad_funcs[SPI_BUS_NUMBER][4] = {
	{
		PAD_FUNC_DOWN(SPIM0_MISO, SPIM0_MI),
		PAD_FUNC_GPIO(SPIM0_CSB),
		PAD_FUNC_DOWN(SPIM0_MOSI, SPIM0_MO),
		PAD_FUNC_DOWN(SPIM0_CLK, SPIM0_CLK),
	},
	{
		PAD_FUNC_DOWN(SPIM1_MISO, SPIM1_MI),
		PAD_FUNC_GPIO(SPIM1_CSB),
		PAD_FUNC_DOWN(SPIM1_MOSI, SPIM1_MO),
		PAD_FUNC_DOWN(SPIM1_CLK, SPIM1_CLK),
	},
	{
		PAD_FUNC_DOWN(SPIM2_MISO, SPIM2_MI),
		PAD_FUNC_GPIO(SPIM2_CSB),
		PAD_FUNC_DOWN(SPIM2_MOSI, SPIM2_MO),
		PAD_FUNC_DOWN(SPIM2_CLK, SPIM2_CLK),
	},
	{
		PAD_FUNC_DOWN(GPIO03, SPIM3_A_MI),
		PAD_FUNC_GPIO(GPIO00),
		PAD_FUNC_DOWN(GPIO02, SPIM3_A_MO),
		PAD_FUNC_DOWN(GPIO01, SPIM3_A_CLK),
	},
	{
		PAD_FUNC_DOWN(GPIO07, SPIM4_A_MI),
		PAD_FUNC_GPIO(GPIO04),
		PAD_FUNC_DOWN(GPIO06, SPIM4_A_MO),
		PAD_FUNC_DOWN(GPIO05, SPIM4_A_CLK),
	},
	{
		PAD_FUNC_DOWN(GPIO11, SPIM5_A_MI),
		PAD_FUNC_GPIO(GPIO08),
		PAD_FUNC_DOWN(GPIO10, SPIM5_A_MO),
		PAD_FUNC_DOWN(GPIO09, SPIM5_A_CLK),
	},
};

static const struct pad_func nor_pinmux[4] = {
	PAD_FUNC(SPINOR_CK, SPINOR_CK, GPIO_PULL_DOWN),
	PAD_FUNC(SPINOR_CS, SPINOR_CS, GPIO_PULL_UP),
	PAD_FUNC(SPINOR_IO0, SPINOR_IO0, GPIO_PULL_DOWN),
	PAD_FUNC(SPINOR_IO1, SPINOR_IO1, GPIO_PULL_DOWN),
};

void mtk_snfc_init(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(nor_pinmux); i++)
		mtk_snfc_init_pad_func(&nor_pinmux[i], GPIO_DRV_8_MA);
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
