/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
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

#include <arch/io.h>
#include <bootblock_common.h>
#include <soc/grf.h>
#include <gpio.h>
#include <soc/i2c.h>
#include <soc/spi.h>
#include <console/console.h>

#include "board.h"

void bootblock_mainboard_early_init(void)
{
	/* Let gpio2ab io domains works at 1.8V.
	 *
	 * If io_vsel[0] == 0(default value), gpio2ab io domains is 3.0V
	 * powerd by APIO2_VDD, otherwise, 1.8V supplied by APIO2_VDDPST.
	 * But from the schematic of kevin rev0, the APIO2_VDD and
	 * APIO2_VDDPST both are 1.8V(intentionally?).
	 *
	 * So, by default, CPU1_SDIO_PWREN(GPIO2_A2) can't output 3.0V
	 * because the supply is 1.8V.
	 * Let ask GPIO2_A2 output 1.8V to make GPIO interal logic happy.
	 */
	write32(&rk3399_grf->io_vsel, RK_SETBITS(1 << 0));

	/*
	 * Let's enable these power rails here, we are already running the SPI
	 * Flash based code.
	 */
	gpio_output(GPIO(0, B, 2), 1);  /* PP1500_EN */
	gpio_output(GPIO(0, B, 4), 1);  /* PP3000_EN */

#if IS_ENABLED(CONFIG_DRIVERS_UART)
	_Static_assert(CONFIG_CONSOLE_SERIAL_UART_ADDRESS == UART2_BASE,
		       "CONSOLE_SERIAL_UART should be UART2");

	/* iomux: select gpio4c[4:3] as uart2 dbg port */
	write32(&rk3399_grf->iomux_uart2c, IOMUX_UART2C);

	/* grf soc_con7[11:10] use for uart2 select */
	write32(&rk3399_grf->soc_con7, UART2C_SEL);
#endif
}

void bootblock_mainboard_init(void)
{
	/* Set pinmux and configure spi flashrom. */
	write32(&rk3399_pmugrf->spi1_rxd, IOMUX_SPI1_RX);
	write32(&rk3399_pmugrf->spi1_csclktx, IOMUX_SPI1_CSCLKTX);
	rockchip_spi_init(CONFIG_BOOT_MEDIA_SPI_BUS, 24750*KHz);

	/* Set pinmux and configure EC SPI. */
	write32(&rk3399_grf->iomux_spi5, IOMUX_SPI5);
	rockchip_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS, 3093750);

	if (IS_ENABLED(CONFIG_GRU_HAS_TPM2)) {
		/* Set pinmux and configure TPM SPI, which is not very fast. */
		write32(&rk3399_grf->iomux_spi0, IOMUX_SPI0);
		rockchip_spi_init(CONFIG_DRIVER_TPM_SPI_BUS, 1500*KHz);
	} else {
		/* Set pinmux and configure TPM I2C */
		write32(&rk3399_pmugrf->iomux_i2c0_scl, IOMUX_I2C0_SCL);
		write32(&rk3399_pmugrf->iomux_i2c0_sda, IOMUX_I2C0_SDA);
		i2c_init(0, 400*KHz);
	}

	setup_chromeos_gpios();
}
