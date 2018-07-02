/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <assert.h>
#include <spi_flash.h>
#include <soc/addressmap.h>
#include <soc/flash_controller.h>
#include <soc/gpio.h>
#include <soc/spi.h>

struct mtk_spi_bus spi_bus[SPI_BUS_NUMBER] = {
	{
		.regs = (void *)SPI0_BASE,
	},
	{
		.regs = (void *)SPI1_BASE,
	},
	{
		.regs = (void *)SPI2_BASE,
	},
	{
		.regs = (void *)SPI3_BASE,
	},
	{
		.regs = (void *)SPI4_BASE,
	},
	{
		.regs = (void *)SPI5_BASE,
	}
};

void mtk_spi_set_gpio_pinmux(unsigned int bus,
			     enum spi_pad_mask pad_select)
{
	switch (bus) {
	case 0:
		if (pad_select == SPI_PAD0_MASK) {
			gpio_set_mode(GPIO(SPI_MI), PAD_SPI_MI_FUNC_SPI0_MI);
			gpio_set_mode(GPIO(SPI_CSB), PAD_SPI_CSB_FUNC_SPI0_CSB);
			gpio_set_mode(GPIO(SPI_MO), PAD_SPI_MO_FUNC_SPI0_MO);
			gpio_set_mode(GPIO(SPI_CLK), PAD_SPI_CLK_FUNC_SPI0_CLK);
		}
		break;
	case 1:
		if (pad_select == SPI_PAD0_MASK) {
			gpio_set_mode(GPIO(SPI1_MI),
					PAD_SPI1_MI_FUNC_SPI1_A_MI);
			gpio_set_mode(GPIO(SPI1_CSB),
					PAD_SPI1_CSB_FUNC_SPI1_A_CSB);
			gpio_set_mode(GPIO(SPI1_MO),
					PAD_SPI1_MO_FUNC_SPI1_A_MO);
			gpio_set_mode(GPIO(SPI1_CLK),
					PAD_SPI1_CLK_FUNC_SPI1_A_CLK);
		}
		if (pad_select == SPI_PAD1_MASK) {
			gpio_set_mode(GPIO(EINT7), PAD_EINT7_FUNC_SPI1_B_MI);
			gpio_set_mode(GPIO(EINT8), PAD_EINT8_FUNC_SPI1_B_CSB);
			gpio_set_mode(GPIO(EINT9), PAD_EINT9_FUNC_SPI1_B_MO);
			gpio_set_mode(GPIO(EINT10), PAD_EINT10_FUNC_SPI1_B_CLK);
		}
		break;
	case 2:
		if (pad_select == SPI_PAD0_MASK) {
			gpio_set_mode(GPIO(KPCOL1), PAD_KPCOL1_FUNC_SPI2_MI);
			gpio_set_mode(GPIO(EINT0), PAD_EINT0_FUNC_SPI2_CSB);
			gpio_set_mode(GPIO(EINT1), PAD_EINT1_FUNC_SPI2_MO);
			gpio_set_mode(GPIO(EINT2), PAD_EINT2_FUNC_SPI2_CLK);
		}
		break;
	case 3:
		if (pad_select == SPI_PAD0_MASK) {
			gpio_set_mode(GPIO(DPI_D8), PAD_DPI_D8_FUNC_SPI3_MI);
			gpio_set_mode(GPIO(DPI_D9), PAD_DPI_D9_FUNC_SPI3_CSB);
			gpio_set_mode(GPIO(DPI_D10), PAD_DPI_D10_FUNC_SPI3_MO);
			gpio_set_mode(GPIO(DPI_D11), PAD_DPI_D11_FUNC_SPI3_CLK);
		}
		break;
	case 4:
		if (pad_select == SPI_PAD0_MASK) {
			gpio_set_mode(GPIO(DPI_D4), PAD_DPI_D4_FUNC_SPI4_MI);
			gpio_set_mode(GPIO(DPI_D5), PAD_DPI_D5_FUNC_SPI4_CSB);
			gpio_set_mode(GPIO(DPI_D6), PAD_DPI_D6_FUNC_SPI4_MO);
			gpio_set_mode(GPIO(DPI_D7), PAD_DPI_D7_FUNC_SPI4_CLK);
		}
		break;
	case 5:
		if (pad_select == SPI_PAD0_MASK) {
			gpio_set_mode(GPIO(DPI_D0), PAD_DPI_D0_FUNC_SPI5_MI);
			gpio_set_mode(GPIO(DPI_D1), PAD_DPI_D1_FUNC_SPI5_CSB);
			gpio_set_mode(GPIO(DPI_D2), PAD_DPI_D2_FUNC_SPI5_MO);
			gpio_set_mode(GPIO(DPI_D3), PAD_DPI_D3_FUNC_SPI5_CLK);
		}
		break;
	}
}

void mtk_spi_set_timing(struct mtk_spi_regs *regs, u32 sck_ticks, u32 cs_ticks)
{
	write32(&regs->spi_cfg0_reg,
		((cs_ticks - 1) << SPI_CFG0_CS_HOLD_SHIFT) |
		((cs_ticks - 1) << SPI_CFG0_CS_SETUP_SHIFT));

	write32(&regs->spi_cfg2_reg,
		((sck_ticks - 1) << SPI_CFG2_SCK_HIGH_SHIFT) |
		((sck_ticks - 1) << SPI_CFG2_SCK_LOW_SHIFT));

	clrsetbits_le32(&regs->spi_cfg1_reg, SPI_CFG1_CS_IDLE_MASK,
			((cs_ticks - 1) << SPI_CFG1_CS_IDLE_SHIFT));
}

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = 0,
		.bus_end = SPI_BUS_NUMBER - 1,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
