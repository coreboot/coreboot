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
#include <vendorcode/google/chromeos/chromeos.h>

void bootblock_mainboard_init(void)
{
	/* i2c1 for tpm*/
	writel(IOMUX_I2C1, &rk3288_grf->iomux_i2c1);

	/* spi2 for firmware ROM */
	writel(IOMUX_SPI2_CSCLK, &rk3288_grf->iomux_spi2csclk);
	writel(IOMUX_SPI2_TXRX, &rk3288_grf->iomux_spi2txrx);
	rockchip_spi_init(CONFIG_BOOT_MEDIA_SPI_BUS);

	/* spi0 for chrome ec */
	writel(IOMUX_SPI0, &rk3288_grf->iomux_spi0);
	rockchip_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS);

	setup_chromeos_gpios();
}
