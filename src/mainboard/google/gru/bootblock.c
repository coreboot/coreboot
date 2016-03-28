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
#include <soc/spi.h>
#include <console/console.h>

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

	if (IS_ENABLED(CONFIG_DRIVERS_UART)) {
		_Static_assert(CONFIG_CONSOLE_SERIAL_UART_ADDRESS == UART2_BASE,
			       "CONSOLE_SERIAL_UART should be UART2");

		/* iomux: select gpio4c[4:3] as uart2 dbg port */
		write32(&rk3399_grf->iomux_uart2c, IOMUX_UART2C);

		/* grf soc_con7[11:10] use for uart2 select */
		write32(&rk3399_grf->soc_con7, UART2C_SEL);
	}
}

void bootblock_mainboard_init(void)
{
	/* select the pinmux for spi flashrom */
	write32(&rk3399_pmugrf->spi1_rxd, IOMUX_SPI1_RX);
	write32(&rk3399_pmugrf->spi1_csclktx, IOMUX_SPI1_CSCLKTX);

	rockchip_spi_init(CONFIG_BOOT_MEDIA_SPI_BUS, 24750*KHz);
}
