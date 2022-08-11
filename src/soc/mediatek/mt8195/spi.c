/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/flash_controller_common.h>
#include <soc/gpio.h>
#include <soc/spi.h>

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
		.cs_gpio = GPIO(PWRAP_SPI_CSN),
	},
	{
		.regs = (void *)SPI4_BASE,
		.cs_gpio = GPIO(DGI_D2),
	},
	{
		.regs = (void *)SPI5_BASE,
		.cs_gpio = GPIO(DGI_D6),
	},
};

struct pad_func {
	u8 pin_id;
	u8 func;
};

#define PAD_FUNC(name, func) {PAD_##name##_ID, PAD_##name##_FUNC_##func}
#define PAD_FUNC_GPIO(name) {PAD_##name##_ID, 0}

static const struct pad_func pad0_funcs[SPI_BUS_NUMBER][4] = {
	{
		PAD_FUNC(SPIM0_MI, SPIM0_MI),
		PAD_FUNC_GPIO(SPIM0_CSB),
		PAD_FUNC(SPIM0_MO, SPIM0_MO),
		PAD_FUNC(SPIM0_CLK, SPIM0_CLK),
	},
	{
		PAD_FUNC(SPIM1_MI, SPIM1_MI),
		PAD_FUNC_GPIO(SPIM1_CSB),
		PAD_FUNC(SPIM1_MO, SPIM1_MO),
		PAD_FUNC(SPIM1_CLK, SPIM1_CLK),
	},
	{
		PAD_FUNC(SPIM2_MI, PIM2_MI),
		PAD_FUNC_GPIO(SPIM2_CSB),
		PAD_FUNC(SPIM2_MO, SPIM2_MO),
		PAD_FUNC(SPIM2_CLK, SPIM2_CLK),
	},
	{
		PAD_FUNC(PWRAP_SPI_MI, SPIM3_MI),
		PAD_FUNC_GPIO(PWRAP_SPI_CSN),
		PAD_FUNC(PWRAP_SPI_MO, SPIM3_MO),
		PAD_FUNC(PWRAP_SPI_CK, SPIM3_CLK),
	},
	{
		PAD_FUNC(DGI_D3, SPIM4_MI),
		PAD_FUNC_GPIO(DGI_D2),
		PAD_FUNC(DGI_D1, SPIM4_MO),
		PAD_FUNC(DGI_D0, SPIM4_CLK),
	},
	{
		PAD_FUNC(DGI_D7, SPIM5_MI),
		PAD_FUNC_GPIO(DGI_D6),
		PAD_FUNC(DGI_D5, SPIM5_MO),
		PAD_FUNC(DGI_D4, SPIM5_CLK),
	},
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
