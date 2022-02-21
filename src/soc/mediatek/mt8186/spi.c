/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 5.6, 5.8
 */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <spi_flash.h>
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
		.cs_gpio = GPIO(SPI2_CSB),
	},
	{
		.regs = (void *)SPI3_BASE,
		.cs_gpio = GPIO(SPI3_CSB),
	},
	{
		.regs = (void *)SPI4_BASE,
		.cs_gpio = GPIO(EINT11),
	},
	{
		.regs = (void *)SPI5_BASE,
		.cs_gpio = GPIO(SPI5_CSB),
	}
};

struct pad_func {
	gpio_t gpio;
	u8 func;
	enum pull_select select;
};

#define PAD_FUNC(name, func) {GPIO(name), PAD_##name##_FUNC_##func, GPIO_PULL_DOWN}
#define PAD_FUNC_SEL(name, func, sel) {GPIO(name), PAD_##name##_FUNC_##func, sel}
#define PAD_FUNC_GPIO(name) {GPIO(name), 0, GPIO_PULL_DOWN}

static const struct pad_func pad0_funcs[SPI_BUS_NUMBER][4] = {
	{
		PAD_FUNC(SPI0_MI, SPI0_MI_A),
		PAD_FUNC_GPIO(SPI0_CSB),
		PAD_FUNC(SPI0_MO, SPI0_MO_A),
		PAD_FUNC(SPI0_CLK, SPI0_CLK_A),
	},
	{
		PAD_FUNC(SPI1_MI, SPI1_MI_A),
		PAD_FUNC_GPIO(SPI1_CSB),
		PAD_FUNC(SPI1_MO, SPI1_MO_A),
		PAD_FUNC(SPI1_CLK, SPI1_CLK_A),
	},
	{
		PAD_FUNC(SPI2_MI, SPI2_MI_A),
		PAD_FUNC_GPIO(SPI2_CSB),
		PAD_FUNC(SPI2_MO, SPI2_MO_A),
		PAD_FUNC(SPI2_CK, SPI2_CLK_A),
	},
	{
		PAD_FUNC(SPI3_MI, SPI3_MI),
		PAD_FUNC_GPIO(SPI3_CSB),
		PAD_FUNC(SPI3_MO, SPI3_MO),
		PAD_FUNC(SPI3_CLK, SPI3_CLK),
	},
	{
		PAD_FUNC(EINT13, SPI4_MI_A),
		PAD_FUNC_GPIO(EINT11),
		PAD_FUNC(EINT12, SPI4_MO_A),
		PAD_FUNC(EINT10, SPI4_CLK_A),
	},
	{
		PAD_FUNC(SPI5_MI, SPI5_MI),
		PAD_FUNC_GPIO(SPI5_CSB),
		PAD_FUNC(SPI5_MO, SPI5_MO),
		PAD_FUNC(SPI5_CLK, SPI5_CLK),
	},
};

static const struct pad_func pad1_funcs[SPI_BUS_NUMBER][4] = {
	{
		PAD_FUNC(EINT3, SPI0_MI_B),
		PAD_FUNC_GPIO(EINT1),
		PAD_FUNC(EINT2, SPI0_MO_B),
		PAD_FUNC(EINT0, SPI0_CLK_B),
	},
	{
		PAD_FUNC(EINT9, SPI1_MI_B),
		PAD_FUNC_GPIO(EINT7),
		PAD_FUNC(EINT8, SPI1_MO_B),
		PAD_FUNC(EINT6, SPI1_CLK_B),
	},
	{
		PAD_FUNC(CAM_PDN1, SPI2_MI_B),
		PAD_FUNC_GPIO(CAM_PDN0),
		PAD_FUNC(CAM_RST0, SPI2_MO_B),
		PAD_FUNC(EINT18, SPI2_CLK_B),
	},
	{
	},
	{
		PAD_FUNC(I2S2_DI, SPI4_MI_B),
		PAD_FUNC_GPIO(I2S2_BCK),
		PAD_FUNC(I2S2_LRCK, SPI4_MO_B),
		PAD_FUNC(I2S2_MCK, SPI4_CLK_B),
	},
	{
	},
};

static const struct pad_func nor_pinmux[SPI_NOR_GPIO_SET_NUM][4] = {
	/* GPIO 36 ~ 39 */
	[SPI_NOR_GPIO_SET0] = {
		PAD_FUNC_SEL(SPI0_CLK, SPINOR_CK, GPIO_PULL_DOWN),
		PAD_FUNC_SEL(SPI0_CSB, SPINOR_CS, GPIO_PULL_UP),
		PAD_FUNC_SEL(SPI0_MO, SPINOR_IO0, GPIO_PULL_DOWN),
		PAD_FUNC_SEL(SPI0_MI, SPINOR_IO1, GPIO_PULL_DOWN),
	},
	/* GPIO 61 ~ 64 */
	[SPI_NOR_GPIO_SET1] = {
		PAD_FUNC_SEL(TDM_RX_BCK, SPINOR_CK, GPIO_PULL_DOWN),
		PAD_FUNC_SEL(TDM_RX_MCLK, SPINOR_CS, GPIO_PULL_UP),
		PAD_FUNC_SEL(TDM_RX_DATA0, SPINOR_IO0, GPIO_PULL_DOWN),
		PAD_FUNC_SEL(TDM_RX_DATA1, SPINOR_IO1, GPIO_PULL_DOWN),
	},
};

void mtk_snfc_init(int gpio_set)
{
	const struct pad_func *ptr = NULL;

	assert(gpio_set < SPI_NOR_GPIO_SET_NUM);

	ptr = nor_pinmux[gpio_set];
	for (size_t i = 0; i < ARRAY_SIZE(nor_pinmux[gpio_set]); i++) {
		gpio_set_pull(ptr[i].gpio, GPIO_PULL_ENABLE, ptr[i].select);
		gpio_set_mode(ptr[i].gpio, ptr[i].func);

		if (gpio_set_driving(ptr[i].gpio, GPIO_DRV_8_MA) < 0)
			printk(BIOS_WARNING,
			       "%s: failed to set pin drive to 8 mA for %d\n",
			       __func__, ptr[i].gpio.id);
		else
			printk(BIOS_DEBUG, "%s: got pin drive: %#x\n", __func__,
			       gpio_get_driving(ptr[i].gpio));
	}
}

void mtk_spi_set_gpio_pinmux(unsigned int bus, enum spi_pad_mask pad_select)
{
	assert(bus < SPI_BUS_NUMBER);
	const struct pad_func *ptr;

	if (pad_select == SPI_PAD0_MASK) {
		ptr = pad0_funcs[bus];
	} else {
		assert((bus == 0 || bus == 1 || bus == 2 || bus == 4) &&
		       pad_select == SPI_PAD1_MASK);
		ptr = pad1_funcs[bus];
	}
	for (int i = 0; i < 4; i++)
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
