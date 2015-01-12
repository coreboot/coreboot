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

#include <arch/cache.h>
#include <arch/io.h>
#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c.h>
#include <edid.h>
#include <gpio.h>
#include <soc/display.h>
#include <soc/grf.h>
#include <soc/soc.h>
#include <soc/pmu.h>
#include <soc/clock.h>
#include <soc/rk808.h>
#include <soc/spi.h>
#include <soc/i2c.h>
#include <symbols.h>
#include <vbe.h>

#include "board.h"

static void configure_usb(void)
{
	gpio_output(GPIO(0, B, 3), 1);			/* HOST1_PWR_EN */
	gpio_output(GPIO(0, B, 4), 1);			/* USBOTG_PWREN_H */
	gpio_output(GPIO(7, C, 5), 1);			/* 5V_DRV */
}

static void configure_sdmmc(void)
{
	writel(IOMUX_SDMMC0, &rk3288_grf->iomux_sdmmc0);

	/* use sdmmc0 io, disable JTAG function */
	writel(RK_CLRBITS(1 << 12), &rk3288_grf->soc_con0);

	/* Note: these power rail definitions are copied in romstage.c */
	rk808_configure_ldo(4, 3300); /* VCCIO_SD */
	rk808_configure_ldo(5, 3300); /* VCC33_SD */

	gpio_input(GPIO(7, A, 5));		/* SD_DET */
}

static void configure_emmc(void)
{
	writel(IOMUX_EMMCDATA, &rk3288_grf->iomux_emmcdata);
	writel(IOMUX_EMMCPWREN, &rk3288_grf->iomux_emmcpwren);
	writel(IOMUX_EMMCCMD, &rk3288_grf->iomux_emmccmd);

	gpio_output(GPIO(2, B, 1), 1);		/* EMMC_RST_L */
}

static void configure_codec(void)
{
	writel(IOMUX_I2C2, &rk3288_grf->iomux_i2c2);	/* CODEC I2C */
	i2c_init(2, 400*KHz);				/* CODEC I2C */

	writel(IOMUX_I2S, &rk3288_grf->iomux_i2s);
	writel(IOMUX_I2SCLK, &rk3288_grf->iomux_i2sclk);

	rk808_configure_ldo(6, 1800);	/* VCC18_CODEC */

	/* AUDIO IO domain 1.8V voltage selection */
	writel(RK_SETBITS(1 << 6), &rk3288_grf->io_vsel);
	rkclk_configure_i2s(12288000);
}

static void configure_vop(void)
{
	writel(IOMUX_LCDC, &rk3288_grf->iomux_lcdc);

	/* lcdc(vop) iodomain select 1.8V */
	writel(RK_SETBITS(1 << 0), &rk3288_grf->io_vsel);

	switch (board_id()) {
	case 0:
		rk808_configure_switch(2, 1);	/* VCC18_LCD */
		rk808_configure_ldo(7, 2500);	/* VCC10_LCD_PWREN_H */
		rk808_configure_switch(1, 1);	/* VCC33_LCD */
		break;
	default:
		gpio_output(GPIO(2, B, 5), 1);	/* AVDD_1V8_DISP_EN */
		rk808_configure_ldo(7, 2500);	/* VCC10_LCD_PWREN_H */
		gpio_output(GPIO(7, B, 6), 1);	/* LCD_EN */
		rk808_configure_switch(1, 1);	/* VCC33_LCD */
		break;
	}
}

static void mainboard_init(device_t dev)
{
	gpio_output(GPIO_RESET, 0);

	configure_usb();
	configure_sdmmc();
	configure_emmc();
	configure_codec();
	configure_vop();
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
	dma->range_start = (uintptr_t)_dma_coherent;
	dma->range_size = _dma_coherent_size;
}

void mainboard_power_on_backlight(void)
{
	switch (board_id()) {
	case 0:
		gpio_output(GPIO(7, A, 0), 0);	/* BL_EN */
		gpio_output(GPIO(7, A, 2), 1);	/* LCD_BL */
		mdelay(10);
		gpio_output(GPIO(7, A, 0), 1);	/* BL_EN */
		break;
	default:
		gpio_output(GPIO(2, B, 4), 1);	/* BL_PWR_EN */
		mdelay(10);
		gpio_output(GPIO(7, A, 2), 1);	/* LCD_BL */
		mdelay(10);
		gpio_output(GPIO(7, A, 0), 1);	/* BL_EN */
		break;
	}
}
