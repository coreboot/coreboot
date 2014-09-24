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

#define DRAM_START	(CONFIG_SYS_SDRAM_BASE >> 20)
#define DRAM_SIZE	CONFIG_DRAM_SIZE_MB
#define DRAM_END	(DRAM_START + DRAM_SIZE)

static void setup_gpio(void)
{
	/*SOC and TPM reset GPIO, active high.*/
	gpio_output(GPIO(0, B, 2), 0);

	/* Configure GPIO for lcd_bl_en */
	gpio_output(GPIO(7, A, 2), 1);

	/*Configure backlight PWM 100% brightness*/
	gpio_output(GPIO(7, A, 0), 0);

	/* Configure GPIO for lcd_en */
	gpio_output(GPIO(7, B, 7), 1);
}

static void setup_iomux(void)
{
	/*i2c0 for pmic*/
	setbits_le32(&rk3288_pmu->iomux_i2c0scl, IOMUX_I2C0SCL);
	setbits_le32(&rk3288_pmu->iomux_i2c0sda, IOMUX_I2C0SDA);

	/*i2c2 for codec*/
	writel(IOMUX_I2C2, &rk3288_grf->iomux_i2c2);

	writel(IOMUX_I2S, &rk3288_grf->iomux_i2s);
	writel(IOMUX_I2SCLK, &rk3288_grf->iomux_i2sclk);
	writel(IOMUX_LCDC, &rk3288_grf->iomux_lcdc);
	writel(IOMUX_SDMMC0, &rk3288_grf->iomux_sdmmc0);
	writel(IOMUX_EMMCDATA, &rk3288_grf->iomux_emmcdata);
	writel(IOMUX_EMMCPWREN, &rk3288_grf->iomux_emmcpwren);
	writel(IOMUX_EMMCCMD, &rk3288_grf->iomux_emmccmd);
}

static void setup_usb_poweron(void)
{
	/* Configure GPIO for usb1_pwr_en */
	gpio_output(GPIO(0, B, 3), 1);

	/* Configure GPIO for usb2_pwr_en */
	gpio_output(GPIO(0, B, 4), 1);

	/* Configure GPIO for 5v_drv */
	gpio_output(GPIO(7, B, 3), 1);
}

static void configure_sdmmc(void)
{
	/* Configure GPIO for sd_en */
	gpio_output(GPIO(7, C, 5), 1);

	/* Configure GPIO for sd_detec */
	gpio_input_pullup(GPIO(7, A, 5));

	/*use sdmmc0 io, disable JTAG function*/
	writel(RK_CLRBITS(1 << 12), &rk3288_grf->soc_con0);
}

static void configure_emmc(void)
{
	/* Configure GPIO for emmc_pwrctrl */
	gpio_output(GPIO(7, B, 4), 1);
}

static void configure_i2s(void)
{
	/*AUDIO IO domain 1.8V voltage selection*/
	writel(RK_SETBITS(1 << 6), &rk3288_grf->io_vsel);
	rkclk_configure_i2s(12288000);
}

static void pmic_init(unsigned int bus)
{
	rk808_configure_ldo(bus, 4, 1800);	/* VCC18_LCD */
	rk808_configure_ldo(bus, 5, 1800);	/* VCC18_CODEC */
	rk808_configure_ldo(bus, 6, 1000);	/* VCC10_LCD */
	rk808_configure_ldo(bus, 8, 3300);	/* VCCIO_SD */
}

static void mainboard_init(device_t dev)
{
	setup_iomux();
	pmic_init(0);
	setup_gpio();
	setup_usb_poweron();
	configure_sdmmc();
	configure_emmc();
	configure_i2s();
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
