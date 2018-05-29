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
 */

#include <arch/cache.h>
#include <arch/io.h>
#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c_simple.h>
#include <edid.h>
#include <elog.h>
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
#include <vendorcode/google/chromeos/chromeos.h>

#include "board.h"

static void configure_usb(void)
{
	gpio_output(GPIO(0, B, 3), 1);			/* HOST1_PWR_EN */
	gpio_output(GPIO(0, B, 4), 1);			/* USBOTG_PWREN_H */
	gpio_output(GPIO(7, C, 5), 1);			/* 5V_DRV */
}

static void configure_sdmmc(void)
{
	write32(&rk3288_grf->iomux_sdmmc0, IOMUX_SDMMC0);

	/* use sdmmc0 io, disable JTAG function */
	write32(&rk3288_grf->soc_con0, RK_CLRBITS(1 << 12));

	/* Note: these power rail definitions are copied in romstage.c */
	rk808_configure_ldo(4, 3300); /* VCCIO_SD */
	rk808_configure_ldo(5, 3300); /* VCC33_SD */

	gpio_input(GPIO(7, A, 5));		/* SD_DET */
}

static void configure_emmc(void)
{
	write32(&rk3288_grf->iomux_emmcdata, IOMUX_EMMCDATA);
	write32(&rk3288_grf->iomux_emmcpwren, IOMUX_EMMCPWREN);
	write32(&rk3288_grf->iomux_emmccmd, IOMUX_EMMCCMD);

	gpio_output(GPIO(2, B, 1), 1);		/* EMMC_RST_L */
}

static void configure_codec(void)
{
	write32(&rk3288_grf->iomux_i2c2, IOMUX_I2C2);	/* CODEC I2C */
	i2c_init(2, 400*KHz);				/* CODEC I2C */

	write32(&rk3288_grf->iomux_i2s, IOMUX_I2S);
	write32(&rk3288_grf->iomux_i2sclk, IOMUX_I2SCLK);

	rk808_configure_ldo(6, 1800);	/* VCC18_CODEC */

	/* AUDIO IO domain 1.8V voltage selection */
	write32(&rk3288_grf->io_vsel, RK_SETBITS(1 << 6));
	rkclk_configure_i2s(12288000);
}

static void configure_vop(void)
{
	write32(&rk3288_grf->iomux_lcdc, IOMUX_LCDC);

	/* lcdc(vop) iodomain select 1.8V */
	write32(&rk3288_grf->io_vsel, RK_SETBITS(1 << 0));

	gpio_output(GPIO(2, B, 5), 1);	/* AVDD_1V8_DISP_EN */
	rk808_configure_ldo(7, 2500);	/* VCC10_LCD_PWREN_H */
	gpio_output(GPIO(7, B, 6), 1);	/* LCD_EN */
	rk808_configure_switch(1, 1);	/* VCC33_LCD */

	/* enable edp HPD */
	gpio_input_pulldown(GPIO(7, B, 3));
	write32(&rk3288_grf->iomux_edp_hotplug, IOMUX_EDP_HOTPLUG);
}

static void mainboard_init(struct device *dev)
{
	gpio_output(GPIO_RESET, 0);

	configure_usb();
	configure_sdmmc();
	configure_emmc();
	configure_codec();
	configure_vop();
}

static void mainboard_enable(struct device *dev)
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
	gpio_output(GPIO(2, B, 4), 1);	/* BL_PWR_EN */
	mdelay(120);
	gpio_output(GPIO(7, A, 0), 1);	/* LCD_BL */
	mdelay(10);
	gpio_output(GPIO_BACKLIGHT, 1);	/* BL_EN */
}
