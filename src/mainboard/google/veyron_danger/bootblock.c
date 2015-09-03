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
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <assert.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <delay.h>
#include <reset.h>
#include <soc/clock.h>
#include <soc/i2c.h>
#include <soc/grf.h>
#include <soc/pmu.h>
#include <soc/rk808.h>
#include <soc/spi.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "board.h"

void bootblock_mainboard_early_init()
{
	if (IS_ENABLED(CONFIG_DRIVERS_UART)) {
		assert(CONFIG_CONSOLE_SERIAL_UART_ADDRESS == UART2_BASE);
		write32(&rk3288_grf->iomux_uart2, IOMUX_UART2);
	}

}

void bootblock_mainboard_init(void)
{
	if (rkclk_was_watchdog_reset())
		reboot_from_watchdog();

	gpio_output(GPIO(7, A, 0), 1);	/* Power LED */

	/* Up VDD_CPU (BUCK1) to 1.4V to support max CPU frequency (1.8GHz). */
	setbits_le32(&rk3288_pmu->iomux_i2c0scl, IOMUX_I2C0SCL);
	setbits_le32(&rk3288_pmu->iomux_i2c0sda, IOMUX_I2C0SDA);
	assert(CONFIG_PMIC_BUS == 0);	/* must correspond with IOMUX */
	i2c_init(CONFIG_PMIC_BUS, 400*KHz);

	/* Slowly raise to max CPU voltage to prevent overshoot */
	rk808_configure_buck(1, 1200);
	udelay(175);/* Must wait for voltage to stabilize,2mV/us */
	rk808_configure_buck(1, 1400);
	udelay(100);/* Must wait for voltage to stabilize,2mV/us */
	rkclk_configure_cpu(APLL_1800_MHZ);

	/* i2c1 for tpm */
	write32(&rk3288_grf->iomux_i2c1, IOMUX_I2C1);
	i2c_init(1, 400*KHz);

	/* spi2 for firmware ROM */
	write32(&rk3288_grf->iomux_spi2csclk, IOMUX_SPI2_CSCLK);
	write32(&rk3288_grf->iomux_spi2txrx, IOMUX_SPI2_TXRX);
	rockchip_spi_init(CONFIG_BOOT_MEDIA_SPI_BUS, 24750*KHz);

	setup_chromeos_gpios();
}
