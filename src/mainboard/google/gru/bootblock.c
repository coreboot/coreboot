/*
 * This file is part of the coreboot project.
 *
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
 */

#include <device/mmio.h>
#include <bootblock_common.h>
#include <delay.h>
#include <soc/grf.h>
#include <gpio.h>
#include <soc/clock.h>
#include <soc/i2c.h>
#include <soc/pwm.h>
#include <soc/spi.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "board.h"
#include "pwm_regulator.h"

void bootblock_mainboard_early_init(void)
{
	/* Configure all programmable IO voltage domains (3D/4A and 2A/2B) early
	   so that we know we can use our GPIOs reliably in following code. */
	write32(&rk3399_grf->io_vsel, RK_SETBITS(1 << 1 | 1 << 0));
	/* On Scarlet-based boards, the 4C/4D domain is 1.8V (on others 3.0V) */
	if (CONFIG(GRU_BASEBOARD_SCARLET))
		write32(&rk3399_grf->io_vsel, RK_SETBITS(1 << 3));

	/* Reconfigure GPIO1 from dynamic voltage selection through GPIO0_B1 to
	   hardcoded 1.8V, and change that pin to a normal GPIO. The TRM says
	   this is already the power-on reset, but we all know that TRMs lie. */
	write32(&rk3399_pmugrf->soc_con0, RK_SETBITS(1 << 9 | 1 << 8));
	write32(&rk3399_pmugrf->gpio0b_iomux, RK_CLRBITS(3 << 2));

	/* Enable rails powering GPIO blocks, among other things. */
	gpio_output(GPIO_P30V_EN, 1);
	if (!CONFIG(GRU_BASEBOARD_SCARLET))
		gpio_output(GPIO_P15V_EN, 1);	/* Scarlet: EC-controlled */

	if (CONFIG(CONSOLE_SERIAL)) {
		_Static_assert(CONFIG_CONSOLE_SERIAL_UART_ADDRESS == UART2_BASE,
			       "CONSOLE_SERIAL_UART should be UART2");

		/* iomux: select gpio4c[4:3] as uart2 dbg port */
		write32(&rk3399_grf->iomux_uart2c, IOMUX_UART2C);

		/* grf soc_con7[11:10] use for uart2 select */
		write32(&rk3399_grf->soc_con7, UART2C_SEL);
	}
}

static void configure_spi_flash(void)
{
	gpio_input(GPIO(1, A, 7));	/* SPI1_MISO remove pull-up */
	gpio_input(GPIO(1, B, 0));	/* SPI1_MOSI remove pull-up */
	gpio_input(GPIO(1, B, 1));	/* SPI1_CLK remove pull-up */
	gpio_input(GPIO(1, B, 2));	/* SPI1_CS remove pull-up */

	rockchip_spi_init(CONFIG_BOOT_DEVICE_SPI_FLASH_BUS, 33*MHz);
	rockchip_spi_set_sample_delay(CONFIG_BOOT_DEVICE_SPI_FLASH_BUS, 5);

	write32(&rk3399_pmugrf->spi1_rxd, IOMUX_SPI1_RX);
	write32(&rk3399_pmugrf->spi1_csclktx, IOMUX_SPI1_CSCLKTX);
}

static void configure_ec(void)
{
	gpio_input(GPIO(2, C, 4));	/* SPI5_MISO remove pull-up */
	gpio_input(GPIO(2, C, 5));	/* SPI5_MOSI remove pull-up */
	gpio_input(GPIO(2, C, 6));	/* SPI5_CLK remove pull-up */
	gpio_input_pullup(GPIO(2, C, 7));	/* SPI5_CS confirm pull-up */

	rockchip_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS, 3093750);

	write32(&rk3399_grf->iomux_spi5, IOMUX_SPI5);
}

static void configure_tpm(void)
{
	if (CONFIG(GRU_HAS_TPM2)) {
		rockchip_spi_init(CONFIG_DRIVER_TPM_SPI_BUS, 1500*KHz);

		if (CONFIG(GRU_BASEBOARD_SCARLET)) {
			gpio_input(GPIO(2, B, 1));	/* SPI2_MISO no-pull */
			gpio_input(GPIO(2, B, 2));	/* SPI2_MOSI no-pull */
			gpio_input(GPIO(2, B, 3));	/* SPI2_CLK no-pull */
			gpio_input_pullup(GPIO(2, B, 4));	/* SPI2_CS */
			write32(&rk3399_grf->iomux_spi2, IOMUX_SPI2);
		} else {
			gpio_input(GPIO(3, A, 4));	/* SPI0_MISO no-pull */
			gpio_input(GPIO(3, A, 5));	/* SPI0_MOSI no-pull */
			gpio_input(GPIO(3, A, 6));	/* SPI0_CLK no-pull */
			gpio_input_pullup(GPIO(3, A, 7));	/* SPI0_CS */
			write32(&rk3399_grf->iomux_spi0, IOMUX_SPI0);
		}

		gpio_input_irq(GPIO_TPM_IRQ, IRQ_TYPE_EDGE_RISING, GPIO_PULLUP);
	} else {
		gpio_input(GPIO(1, B, 7));	/* I2C0_SDA remove pull-up */
		gpio_input(GPIO(1, C, 0));	/* I2C0_SCL remove pull-up */

		i2c_init(0, 400*KHz);

		write32(&rk3399_pmugrf->iomux_i2c0_scl, IOMUX_I2C0_SCL);
		write32(&rk3399_pmugrf->iomux_i2c0_sda, IOMUX_I2C0_SDA);
	}
}

static void speed_up_boot_cpu(void)
{
	pwm_regulator_configure(PWM_REGULATOR_LIT, 1150);

	udelay(200);

	rkclk_configure_cpu(APLL_1512_MHZ, CPU_CLUSTER_LITTLE);
}

void bootblock_mainboard_init(void)
{
	speed_up_boot_cpu();

	if (rkclk_was_watchdog_reset())
		reboot_from_watchdog();

	configure_spi_flash();
	configure_ec();
	configure_tpm();

	setup_chromeos_gpios();
}
