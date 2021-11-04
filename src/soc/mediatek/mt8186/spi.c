/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 5.6, 5.8
 */

#include <assert.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/flash_controller_common.h>
#include <soc/gpio.h>
#include <soc/spi.h>

struct pad_func {
	gpio_t gpio;
	u8 func;
};

#define PAD_FUNC(name, func) {GPIO(name), PAD_##name##_FUNC_##func}

static const struct pad_func nor_pinmux[SPI_NOR_GPIO_SET_NUM][4] = {
	/* GPIO 36 ~ 39 */
	[SPI_NOR_GPIO_SET0] = {
		PAD_FUNC(SPI0_CLK, SPINOR_CK),
		PAD_FUNC(SPI0_CSB, SPINOR_CS),
		PAD_FUNC(SPI0_MO, SPINOR_IO0),
		PAD_FUNC(SPI0_MI, SPINOR_IO1),
	},
	/* GPIO 61 ~ 64 */
	[SPI_NOR_GPIO_SET1] = {
		PAD_FUNC(TDM_RX_BCK, SPINOR_CK),
		PAD_FUNC(TDM_RX_MCLK, SPINOR_CS),
		PAD_FUNC(TDM_RX_DATA0, SPINOR_IO0),
		PAD_FUNC(TDM_RX_DATA1, SPINOR_IO1),
	},
};

void mtk_snfc_init(int gpio_set)
{
	const struct pad_func *ptr = NULL;

	assert(gpio_set < SPI_NOR_GPIO_SET_NUM);

	ptr = nor_pinmux[gpio_set];
	for (size_t i = 0; i < ARRAY_SIZE(nor_pinmux[gpio_set]); i++) {
		gpio_set_pull(ptr[i].gpio, GPIO_PULL_ENABLE, GPIO_PULL_UP);
		gpio_set_mode(ptr[i].gpio, ptr[i].func);
	}
}

static const struct spi_ctrlr spi_flash_ctrlr = {
	.max_xfer_size = 65535,
	.flash_probe = mtk_spi_flash_probe,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_flash_ctrlr,
		.bus_start = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS,
		.bus_end = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
