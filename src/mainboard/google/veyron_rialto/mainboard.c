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
#include <security/vboot/vboot_common.h>

#include "board.h"

static void configure_usb(void)
{
	gpio_output(GPIO(0, B, 4), 1);			/* USB2_PWR_EN */
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

static void configure_3g(void)
{
	/* Force 3G modem off to avoid confusing the EHCI host and
	 * causing problems during enumeration/init */
	gpio_output(GPIO(5, C, 1), 1);	/* 3G_SHUTDOWN */
	gpio_output(GPIO(4, D, 2), 0);	/* 3G_ON_OFF */
}

static void mainboard_init(struct device *dev)
{
	gpio_output(GPIO_RESET, 0);

	configure_usb();
	configure_emmc();
	configure_codec();
	configure_3g();
	/* No video. */

	/* If recovery mode is detected, reduce frequency and voltage to reduce
	 * heat in case machine is left unattended. chrome-os-partner:41201. */
	if (vboot_recovery_mode_enabled())  {
		printk(BIOS_DEBUG, "Reducing APLL freq for recovery mode.\n");
		rkclk_configure_cpu(APLL_600_MHZ);
		rk808_configure_buck(1, 900);
	}
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
	return;
}
