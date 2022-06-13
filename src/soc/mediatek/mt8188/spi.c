/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 5.8, 5.19
 */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/flash_controller_common.h>
#include <soc/gpio.h>
#include <soc/spi.h>

struct pad_func {
	gpio_t gpio;
	u8 func;
	enum pull_select select;
};

#define PAD_FUNC_SEL(name, func, sel) {GPIO(name), PAD_##name##_FUNC_##func, sel}

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
