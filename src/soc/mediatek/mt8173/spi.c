/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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
		.regs = (void *)SPI_BASE,
	}
};

void mtk_spi_set_gpio_pinmux(unsigned int bus,
			     enum spi_pad_mask pad_select)
{
	/* TODO: implement support for other pads when needed */
	assert(pad_select == SPI_PAD1_MASK);
	gpio_set_mode(GPIO(MSDC2_DAT2), PAD_MSDC2_DAT2_FUNC_SPI_CK_1);
	gpio_set_mode(GPIO(MSDC2_DAT3), PAD_MSDC2_DAT3_FUNC_SPI_MI_1);
	gpio_set_mode(GPIO(MSDC2_CLK), PAD_MSDC2_CLK_FUNC_SPI_MO_1);
	gpio_set_mode(GPIO(MSDC2_CMD), PAD_MSDC2_CMD_FUNC_SPI_CS_1);
}

void mtk_spi_set_timing(struct mtk_spi_regs *regs, u32 sck_ticks, u32 cs_ticks)
{
	write32(&regs->spi_cfg0_reg,
		((sck_ticks - 1) << SPI_CFG0_SCK_HIGH_SHIFT) |
		((sck_ticks - 1) << SPI_CFG0_SCK_LOW_SHIFT) |
		((cs_ticks - 1) << SPI_CFG0_CS_HOLD_SHIFT) |
		((cs_ticks - 1) << SPI_CFG0_CS_SETUP_SHIFT));
	clrsetbits_le32(&regs->spi_cfg1_reg, SPI_CFG1_CS_IDLE_MASK,
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
