/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
 * Copyright 2014 Google Inc.
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

#include <arch/io.h>
#include <bootblock_common.h>
#include <soc/clock.h>
#include <soc/i2c.h>
#include <soc/grf.h>
#include <soc/pmu.h>
#include <soc/rk808.h>
#include <soc/spi.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <delay.h>

#include "board.h"

void bootblock_mainboard_init(void)
{
	/* cpu frequency will up to 1.8GHz,
	 * in our experience the buck1
	 * must up to 1.4v
	 */
	setbits_le32(&rk3288_pmu->iomux_i2c0scl, IOMUX_I2C0SCL);
	setbits_le32(&rk3288_pmu->iomux_i2c0sda, IOMUX_I2C0SDA);
	i2c_init(PMIC_BUS, 400*KHz);

	/* Slowly raise to max CPU voltage to prevent overshoot */
	rk808_configure_buck(PMIC_BUS, 1, 1200);
	udelay(175);/* Must wait for voltage to stabilize,2mV/us */
	rk808_configure_buck(PMIC_BUS, 1, 1400);
	udelay(100);/* Must wait for voltage to stabilize,2mV/us */
	rkclk_configure_cpu();

	/* i2c1 for tpm */
	writel(IOMUX_I2C1, &rk3288_grf->iomux_i2c1);

	/* spi2 for firmware ROM */
	writel(IOMUX_SPI2_CSCLK, &rk3288_grf->iomux_spi2csclk);
	writel(IOMUX_SPI2_TXRX, &rk3288_grf->iomux_spi2txrx);
	rockchip_spi_init(CONFIG_BOOT_MEDIA_SPI_BUS, 11*MHz);

	/* spi0 for chrome ec */
	writel(IOMUX_SPI0, &rk3288_grf->iomux_spi0);
	rockchip_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS, 9*MHz);

	setup_chromeos_gpios();
}
