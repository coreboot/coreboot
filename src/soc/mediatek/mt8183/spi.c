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

#include <device/mmio.h>
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

struct pad_func {
	u8 pin_id;
	u8 func;
};

#define PAD_FUNC(name, func) {PAD_##name##_ID, PAD_##name##_FUNC_##func}

static const struct pad_func pad0_funcs[SPI_BUS_NUMBER][4] = {
	{
		PAD_FUNC(SPI_MI, SPI0_MI),
		PAD_FUNC(SPI_CSB, SPI0_CSB),
		PAD_FUNC(SPI_MO, SPI0_MO),
		PAD_FUNC(SPI_CLK, SPI0_CLK),
	},
	{
		PAD_FUNC(SPI1_MI, SPI1_A_MI),
		PAD_FUNC(SPI1_CSB, SPI1_A_CSB),
		PAD_FUNC(SPI1_MO, SPI1_A_MO),
		PAD_FUNC(SPI1_CLK, SPI1_A_CLK),
	},
	{
		PAD_FUNC(KPCOL1, SPI2_MI),
		PAD_FUNC(EINT0, SPI2_CSB),
		PAD_FUNC(EINT1, SPI2_MO),
		PAD_FUNC(EINT2, SPI2_CLK),
	},
	{
		PAD_FUNC(DPI_D8, SPI3_MI),
		PAD_FUNC(DPI_D9, SPI3_CSB),
		PAD_FUNC(DPI_D10, SPI3_MO),
		PAD_FUNC(DPI_D11, SPI3_CLK),
	},
	{
		PAD_FUNC(DPI_D4, SPI4_MI),
		PAD_FUNC(DPI_D5, SPI4_CSB),
		PAD_FUNC(DPI_D6, SPI4_MO),
		PAD_FUNC(DPI_D7, SPI4_CLK),
	},
	{
		PAD_FUNC(DPI_D0, SPI5_MI),
		PAD_FUNC(DPI_D1, SPI5_CSB),
		PAD_FUNC(DPI_D2, SPI5_MO),
		PAD_FUNC(DPI_D3, SPI5_CLK),
	}
};

static const struct pad_func bus1_pad1_funcs[4] = {
	PAD_FUNC(EINT7, SPI1_B_MI),
	PAD_FUNC(EINT8, SPI1_B_CSB),
	PAD_FUNC(EINT9, SPI1_B_MO),
	PAD_FUNC(EINT10, SPI1_B_CLK),
};

void mtk_spi_set_gpio_pinmux(unsigned int bus, enum spi_pad_mask pad_select)
{
	assert(bus < SPI_BUS_NUMBER);
	const struct pad_func *ptr;
	if (pad_select == SPI_PAD0_MASK) {
		ptr = pad0_funcs[bus];
	} else {
		assert(bus == 1 && pad_select == SPI_PAD1_MASK);
		ptr = bus1_pad1_funcs;
	}
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

	clrsetbits_le32(&regs->spi_cfg1_reg, SPI_CFG1_TICK_DLY_MASK |
			SPI_CFG1_CS_IDLE_MASK,
			(tick_dly << SPI_CFG1_TICK_DLY_SHIFT) |
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
