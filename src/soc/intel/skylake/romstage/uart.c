/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation
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
#include <console/uart.h>
#include <device/pci_def.h>
#include <stdint.h>
#include <soc/pci_devs.h>
#include <soc/pcr.h>
#include <soc/romstage.h>
#include <soc/serialio.h>
#include <gpio.h>

/* UART2 pad configuration. Support RXD and TXD for now. */
static const struct pad_config uart2_pads[] = {
/* UART2_RXD */		PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
/* UART2_TXD */		PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
};

void pch_uart_init(void)
{
	device_t dev = PCH_DEV_UART2;
	u32 tmp;
	u8 *base = (void *)uart_platform_base(CONFIG_UART_FOR_CONSOLE);

	/* Set configured UART2 base address */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, (u32)base);

	/* Enable memory access and bus master */
	tmp = pci_read_config32(dev, PCI_COMMAND);
	tmp |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
	pci_write_config32(dev, PCI_COMMAND, tmp);

	/* Take UART2 out of reset */
	tmp = read32(base + SIO_REG_PPR_RESETS);
	tmp |= SIO_REG_PPR_RESETS_FUNC | SIO_REG_PPR_RESETS_APB |
		SIO_REG_PPR_RESETS_IDMA;
	write32(base + SIO_REG_PPR_RESETS, tmp);

	/*
	 * Set M and N divisor inputs and enable clock.
	 * Main reference frequency to UART is:
	 *  120MHz * M / N = 120MHz * 48 / 3125 = 1843200Hz
	 */
	tmp = read32(base + SIO_REG_PPR_CLOCK);
	tmp |= SIO_REG_PPR_CLOCK_EN | SIO_REG_PPR_CLOCK_UPDATE |
		(SIO_REG_PPR_CLOCK_N_DIV << 16) |
		(SIO_REG_PPR_CLOCK_M_DIV << 1);
	write32(base + SIO_REG_PPR_CLOCK, tmp);

	/* Put UART2 in byte access mode for 16550 compatibility */
	if (!IS_ENABLED(CONFIG_DRIVERS_UART_8250MEM_32))
		pcr_andthenor32(PID_SERIALIO,
		R_PCH_PCR_SERIAL_IO_GPPRVRW7, 0, SIO_PCH_LEGACY_UART2);

	gpio_configure_pads(uart2_pads, ARRAY_SIZE(uart2_pads));
}
