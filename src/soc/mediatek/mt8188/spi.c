/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 5.8, 5.19
 */

#include <assert.h>
#include <device/mmio.h>
#include <gpio.h>
#include <soc/addressmap.h>
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
		.cs_gpio = GPIO(DPI_D12),
	},
	{
		.regs = (void *)SPI4_BASE,
		.cs_gpio = GPIO(GPIO12),
	},
	{
		.regs = (void *)SPI5_BASE,
		.cs_gpio = GPIO(GPIO00),
	},
};

struct pad_func {
	gpio_t gpio;
	u8 func;
	enum pull_select select;
};

#define PAD_FUNC_SEL(name, func, sel) {GPIO(name), PAD_##name##_FUNC_##func, sel}
#define PAD_FUNC(name, func) {GPIO(name), PAD_##name##_FUNC_##func}
#define PAD_FUNC_GPIO(name) {GPIO(name), 0}

static const struct pad_func pad_funcs[SPI_BUS_NUMBER][4] = {
	{
		PAD_FUNC(SPIM0_MISO, SPIM0_MISO),
		PAD_FUNC_GPIO(SPIM0_CSB),
		PAD_FUNC(SPIM0_MOSI, SPIM0_MOSI),
		PAD_FUNC(SPIM0_CLK, SPIM0_CLK),
	},
	{
		PAD_FUNC(SPIM1_MISO, SPIM1_MISO),
		PAD_FUNC_GPIO(SPIM1_CSB),
		PAD_FUNC(SPIM1_MOSI, SPIM1_MOSI),
		PAD_FUNC(SPIM1_CLK, SPIM1_CLK),
	},
	{
		PAD_FUNC(SPIM2_MISO, SPIM2_MISO),
		PAD_FUNC_GPIO(SPIM2_CSB),
		PAD_FUNC(SPIM2_MOSI, SPIM2_MOSI),
		PAD_FUNC(SPIM2_CLK, SPIM2_CLK),
	},
	{
		PAD_FUNC(DPI_D15, SPIM3_MISO),
		PAD_FUNC_GPIO(DPI_D12),
		PAD_FUNC(DPI_D14, SPIM3_MOSI),
		PAD_FUNC(DPI_D13, SPIM3_CLK),
	},
	{
		PAD_FUNC(GPIO15, SPIM4_MISO),
		PAD_FUNC_GPIO(GPIO12),
		PAD_FUNC(GPIO14, SPIM4_MOSI),
		PAD_FUNC(GPIO13, SPIM4_CLK),
	},
	{
		PAD_FUNC(GPIO03, SPIM5_MISO),
		PAD_FUNC_GPIO(GPIO00),
		PAD_FUNC(GPIO02, SPIM5_MOSI),
		PAD_FUNC(GPIO01, SPIM5_CLK),
	},
};

static const struct pad_func nor_pinmux[4] = {
	/* GPIO 125 ~ 128 */
	PAD_FUNC_SEL(DMIC1_CLK, SPINOR_CK, GPIO_PULL_DOWN),
	PAD_FUNC_SEL(DMIC1_DAT, SPINOR_CS, GPIO_PULL_UP),
	PAD_FUNC_SEL(DMIC1_DAT_R, SPINOR_IO0, GPIO_PULL_DOWN),
	PAD_FUNC_SEL(DMIC2_CLK, SPINOR_IO1, GPIO_PULL_DOWN),
};

void mtk_snfc_init(void)
{
	const struct pad_func *ptr = NULL;

	ptr = nor_pinmux;
	for (size_t i = 0; i < ARRAY_SIZE(nor_pinmux); i++) {
		gpio_set_pull(ptr[i].gpio, GPIO_PULL_ENABLE, ptr[i].select);
		gpio_set_mode(ptr[i].gpio, ptr[i].func);
	}
}

void mtk_spi_set_gpio_pinmux(unsigned int bus, enum spi_pad_mask pad_select)
{
	assert(bus < SPI_BUS_NUMBER);
	const struct pad_func *ptr;

	ptr = pad_funcs[bus];

	for (unsigned int i = 0; i < SPI_BUS_NUMBER; i++)
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
