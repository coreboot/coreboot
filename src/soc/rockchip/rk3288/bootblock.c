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
#include <arch/cache.h>
#include <arch/io.h>
#include <bootblock_common.h>
#include "timer.h"
#include "clock.h"
#include "grf.h"
#include "spi.h"
#include <vendorcode/google/chromeos/chromeos.h>
#include <soc/rockchip/rk3288/i2c.h>

static void bootblock_cpu_init(void)
{
	writel(IOMUX_UART2, &rk3288_grf->iomux_uart2);
	writel(IOMUX_SPI2_CSCLK, &rk3288_grf->iomux_spi2csclk);
	writel(IOMUX_SPI2_TXRX, &rk3288_grf->iomux_spi2txrx);
	/*i2c1 for tpm*/
	writel(IOMUX_I2C1, &rk3288_grf->iomux_i2c1);

	/* spi0 for chrome ec */
	writel(IOMUX_SPI0, &rk3288_grf->iomux_spi0);
	rk3288_init_timer();
	console_init();
	rkclk_init();

	/*i2c1 for tpm 400khz*/
	i2c_init(1, 400000);
	rockchip_spi_init(CONFIG_BOOT_MEDIA_SPI_BUS);
	rockchip_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS);
	setup_chromeos_gpios();
}
