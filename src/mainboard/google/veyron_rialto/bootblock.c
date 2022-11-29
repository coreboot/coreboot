/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <assert.h>
#include <bootblock_common.h>
#include <delay.h>
#include <soc/clock.h>
#include <soc/i2c.h>
#include <soc/grf.h>
#include <soc/pmu.h>
#include <soc/rk808.h>
#include <soc/spi.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "board.h"

void bootblock_mainboard_early_init(void)
{
	if (CONFIG(CONSOLE_SERIAL)) {
		assert(CONFIG_CONSOLE_SERIAL_UART_ADDRESS == UART2_BASE);
		write32(&rk3288_grf->iomux_uart2, IOMUX_UART2);
	}
}

void bootblock_mainboard_init(void)
{
	if (rkclk_was_watchdog_reset())
		reboot_from_watchdog();

	/* Turn on all leds */
	gpio_output(GPIO(7, A, 0), 1);	/* LED_READY */
	gpio_output(GPIO(7, B, 5), 1);	/* Ready2_LED */
	gpio_output(GPIO(7, B, 3), 1);	/* LED_SYNCING */
	gpio_output(GPIO(7, B, 7), 1);	/* LED_ERROR */

	/* Up VDD_CPU (BUCK1) to 1.4V to support max CPU frequency (1.8GHz). */
	setbits32(&rk3288_pmu->iomux_i2c0scl, IOMUX_I2C0SCL);
	setbits32(&rk3288_pmu->iomux_i2c0sda, IOMUX_I2C0SDA);
	assert(CONFIG_PMIC_BUS == 0);	/* must correspond with IOMUX */
	i2c_init(CONFIG_PMIC_BUS, 400*KHz);

	/* Slowly raise to max CPU voltage to prevent overshoot */
	rk808_configure_buck(1, 1200);
	udelay(175);/* Must wait for voltage to stabilize,2mV/us */
	rkclk_configure_cpu(APLL_1416_MHZ);

	/* i2c1 for tpm */
	write32(&rk3288_grf->iomux_i2c1, IOMUX_I2C1);
	i2c_init(1, 400*KHz);

	/* spi2 for firmware ROM */
	write32(&rk3288_grf->iomux_spi2csclk, IOMUX_SPI2_CSCLK);
	write32(&rk3288_grf->iomux_spi2txrx, IOMUX_SPI2_TXRX);
	rockchip_spi_init(CONFIG_BOOT_DEVICE_SPI_FLASH_BUS, 24750*KHz);

	setup_chromeos_gpios();
}
