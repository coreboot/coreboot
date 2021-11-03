/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <boot/coreboot_tables.h>
#include <device/device.h>
#include <device/i2c_simple.h>
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

#include "board.h"

static void configure_usb(void)
{
	gpio_output(GPIO(0, B, 4), 1);		/* USB2_PWR_EN */
}

static void configure_emmc(void)
{
	write32(&rk3288_grf->iomux_emmcdata, IOMUX_EMMCDATA);
	write32(&rk3288_grf->iomux_emmcpwren, IOMUX_EMMCPWREN);
	write32(&rk3288_grf->iomux_emmccmd, IOMUX_EMMCCMD);

	gpio_output(GPIO(2, B, 1), 1);		/* EMMC_RST_L */
}

static void configure_i2s(void)
{
	write32(&rk3288_grf->iomux_i2s, IOMUX_I2S);
	write32(&rk3288_grf->iomux_i2sclk, IOMUX_I2SCLK);

	/* AUDIO IO domain 1.8V voltage selection */
	write32(&rk3288_grf->io_vsel, RK_SETBITS(1 << 6));
	rkclk_configure_i2s(12288000);
}

static void configure_vop(void)
{
	write32(&rk3288_grf->iomux_lcdc, IOMUX_LCDC);

	/* lcdc(vop) iodomain select 1.8V */
	write32(&rk3288_grf->io_vsel, RK_SETBITS(1 << 0));

	rk808_configure_ldo(8, 1800);	/* VCC18_LCD (HDMI_AVDD_1V8) */
	rk808_configure_ldo(7, 1000);	/* VDD10_LCD (HDMI_AVDD_1V0) */
	rk808_configure_switch(1, 1);	/* VCC33_LCD */
}

static void configure_hdmi(void)
{
	gpio_output(GPIO(7, B, 3), 1);	/* POWER_HDMI_ON */
}

static void mainboard_init(struct device *dev)
{
	gpio_output(GPIO_RESET, 0);

	configure_usb();
	configure_emmc();
	configure_i2s();
	configure_vop();
	configure_hdmi();
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
	dma->tag = LB_TAG_DMA;
	dma->size = sizeof(*dma);
	dma->range_start = (uintptr_t)_dma_coherent;
	dma->range_size = REGION_SIZE(dma_coherent);
}

void mainboard_power_on_backlight(void)
{
	return;
}
