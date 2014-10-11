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
#include <soc/rockchip/rk3288/grf.h>
#include <soc/rockchip/rk3288/spi.h>
#include <soc/rockchip/rk3288/rk808.h>
#include <soc/rockchip/rk3288/clock.h>
#include <soc/rockchip/rk3288/pmu.h>
#include <soc/rockchip/rk3288/i2c.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "board.h"

void bootblock_mainboard_init(void)
{
	/* cpu frequency will up to 1.8GHz, so the buck1 must up to 1.3v */
	setbits_le32(&rk3288_pmu->iomux_i2c0scl, IOMUX_I2C0SCL);
	setbits_le32(&rk3288_pmu->iomux_i2c0sda, IOMUX_I2C0SDA);
	i2c_init(PMIC_BUS, 400*KHz);
	rk808_configure_buck(PMIC_BUS, 1, 1300);
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
