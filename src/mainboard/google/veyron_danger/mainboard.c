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
 * Foundation, Inc.
 */

#include <arch/cache.h>
#include <arch/io.h>
#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c.h>
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

	sdmmc_power_on();

	gpio_input(GPIO(7, A, 5));	/* SDMMC_DET_L */
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

	/*
	 * BL_EN gates VCC_LCD. This might be changed in future revisions
	 * of the board so that the display can be stablized before we
	 * turn on the backlight.
	 *
	 * To minimize display corruption, turn off LCDC_BL before
	 * powering on the backlight.
	 */
	switch (board_id()) {
	case 0:
		gpio_output(GPIO(7, A, 3), 1);
		break;
	default:
		gpio_output(GPIO(7, A, 2), 1);
		break;
	}

	gpio_output(GPIO_LCDC_BL, 0);
	rk808_configure_switch(1, 1);	/* VCC33_LCD */

	/* EDP_HPD setup */
	switch (board_id()) {
	case 0:
		/* not present */
		break;
	default:
		/* Unlike other Veyrons, Danger has external pull resistors on
		 * EDP_HPD. Default for GPIO(7, B, 3) is pull-down, set to
		 * float.
		 */
		gpio_input(GPIO(7, B, 3));
		write32(&rk3288_grf->iomux_edp_hotplug, IOMUX_EDP_HOTPLUG);
		break;
	}
}

static void configure_hdmi(void)
{
	rk808_configure_switch(2, 1);	/* VCC18_LCD (HDMI_AVDD_1V8) */
	rk808_configure_ldo(7, 1000);	/* VDD10_LCD (HDMI_AVDD_1V0) */

	/* set POWER_HDMI_EN */
	switch (board_id()) {
	case 0:
		gpio_output(GPIO(7, A, 2), 1);
		break;
	default:
		gpio_output(GPIO(5, C, 3), 1);
		break;
	}

	/* HDMI I2C */
	write32(&rk3288_grf->iomux_i2c5sda, IOMUX_HDMI_EDP_I2C_SDA);
	write32(&rk3288_grf->iomux_i2c5scl, IOMUX_HDMI_EDP_I2C_SCL);
}

static void mainboard_init(device_t dev)
{
	gpio_output(GPIO_RESET, 0);

	configure_usb();
	configure_sdmmc();
	configure_emmc();
	configure_codec();
	configure_vop();
	configure_hdmi();

	elog_init();
	elog_add_watchdog_reset();
	elog_add_boot_reason();
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
	gpio_output(GPIO_LCDC_BL, 1);
}
