/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <arch/cache.h>
#include <delay.h>
#include <edid.h>
#include <vbe.h>
#include <boot/coreboot_tables.h>
#include <device/i2c.h>
#include <soc/rockchip/rk3288/gpio.h>
#include <soc/rockchip/rk3288/soc.h>
#include <soc/rockchip/rk3288/pmu.h>
#include <soc/rockchip/rk3288/clock.h>
#include <soc/rockchip/rk3288/rk808.h>
#include <soc/rockchip/rk3288/spi.h>

#include "board.h"

static void configure_usb(void)
{
	gpio_output(GPIO(0, B, 3), 1);			/* HOST1_PWR_EN */
	gpio_output(GPIO(0, B, 4), 1);			/* USBOTG_PWREN_H */

	switch (board_id()) {
	case 0:
		gpio_output(GPIO(7, B, 3), 1);		/* 5V_DRV */
		break;
	default:
		break;	/* 5V_DRV moved to EC after rev1 */
	}
}

static void configure_sdmmc(void)
{
	writel(IOMUX_SDMMC0, &rk3288_grf->iomux_sdmmc0);

	/* use sdmmc0 io, disable JTAG function */
	writel(RK_CLRBITS(1 << 12), &rk3288_grf->soc_con0);

	switch (board_id()) {
	case 0:
		rk808_configure_ldo(PMIC_BUS, 8, 3300);	/* VCCIO_SD */
		gpio_output(GPIO(7, C, 5), 1);		/* SD_EN */
		break;
	default:
		rk808_configure_ldo(PMIC_BUS, 4, 3300); /* VCCIO_SD */
		rk808_configure_ldo(PMIC_BUS, 5, 3300); /* VCC33_SD */
		break;
	}

	gpio_input(GPIO(7, A, 5));		/* SD_DET */
}

static void configure_emmc(void)
{
	writel(IOMUX_EMMCDATA, &rk3288_grf->iomux_emmcdata);
	writel(IOMUX_EMMCPWREN, &rk3288_grf->iomux_emmcpwren);
	writel(IOMUX_EMMCCMD, &rk3288_grf->iomux_emmccmd);

	gpio_output(GPIO(7, B, 4), 1);			/* EMMC_RST_L */
}

static void configure_codec(void)
{
	writel(IOMUX_I2C2, &rk3288_grf->iomux_i2c2);	/* CODEC I2C */

	writel(IOMUX_I2S, &rk3288_grf->iomux_i2s);
	writel(IOMUX_I2SCLK, &rk3288_grf->iomux_i2sclk);

	switch (board_id()) {
	case 0:
		rk808_configure_ldo(PMIC_BUS, 5, 1800);	/* VCC18_CODEC */
		break;
	default:
		rk808_configure_ldo(PMIC_BUS, 6, 1800);	/* VCC18_CODEC */
		break;
	}

	/* AUDIO IO domain 1.8V voltage selection */
	writel(RK_SETBITS(1 << 6), &rk3288_grf->io_vsel);
	rkclk_configure_i2s(12288000);
}

static void configure_lcd(void)
{
	writel(IOMUX_LCDC, &rk3288_grf->iomux_lcdc);

	switch (board_id()) {
	case 0:
		rk808_configure_ldo(PMIC_BUS, 4, 1800);	/* VCC18_LCD */
		rk808_configure_ldo(PMIC_BUS, 6, 1000);	/* VCC10_LCD */
		gpio_output(GPIO(7, B, 7), 1);		/* LCD_EN */
		break;
	default:
		rk808_configure_switch(PMIC_BUS, 2, 1);	/* VCC18_LCD */
		rk808_configure_ldo(PMIC_BUS, 7, 3300); /* VCC10_LCD_PWREN_H */
		rk808_configure_switch(PMIC_BUS, 1, 1);	/* VCC33_LCD */
		break;
	}

	gpio_output(GPIO(7, A, 0), 0);			/* LCDC_BL */
	gpio_output(GPIO(7, A, 2), 1);			/* BL_EN */
}

static void mainboard_init(device_t dev)
{
	setbits_le32(&rk3288_pmu->iomux_i2c0scl, IOMUX_I2C0SCL); /* PMIC I2C */
	setbits_le32(&rk3288_pmu->iomux_i2c0sda, IOMUX_I2C0SDA); /* PMIC I2C */

	gpio_output(GPIO_RESET, 0);

	configure_usb();
	configure_sdmmc();
	configure_emmc();
	configure_codec();
	configure_lcd();
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

void lb_board(struct lb_header *header)
{
	struct lb_range *dma;

	dma = (struct lb_range *)lb_new_record(header);
	dma->tag = LB_TAB_DMA;
	dma->size = sizeof(*dma);
	dma->range_start = CONFIG_DRAM_DMA_START;
	dma->range_size = CONFIG_DRAM_DMA_SIZE;
}
