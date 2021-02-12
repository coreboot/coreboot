/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <bootblock_common.h>
#include <soc/grf.h>
#include <gpio.h>
#include <soc/spi.h>

void bootblock_mainboard_early_init(void)
{
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
	gpio_input(GPIO(1, A, 7)); /* SPI1_MISO remove pull-up */
	gpio_input(GPIO(1, B, 0)); /* SPI1_MOSI remove pull-up */
	gpio_input(GPIO(1, B, 1)); /* SPI1_CLK remove pull-up */
	gpio_input(GPIO(1, B, 2)); /* SPI1_CS remove pull-up */

	rockchip_spi_init(CONFIG_BOOT_DEVICE_SPI_FLASH_BUS, 33 * MHz);
	rockchip_spi_set_sample_delay(CONFIG_BOOT_DEVICE_SPI_FLASH_BUS, 5);

	write32(&rk3399_pmugrf->spi1_rxd, IOMUX_SPI1_RX);
	write32(&rk3399_pmugrf->spi1_csclktx, IOMUX_SPI1_CSCLKTX);
}

void bootblock_mainboard_init(void)
{
	configure_spi_flash();
}
