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
#include <device/pci_def.h>
#include <stdint.h>
#include <soc/pci_devs.h>
#include <soc/pcr.h>
#include <soc/romstage.h>
#include <soc/serialio.h>

void pch_uart_init(void)
{
	device_t dev;
	u32 tmp, legacy;
	u8 *base = (u8 *)CONFIG_TTYS0_BASE;

	switch (CONFIG_INTEL_PCH_UART_CONSOLE_NUMBER) {
	case 0:
		dev = PCH_DEV_UART0;
		legacy = SIO_PCH_LEGACY_UART0;
		break;
	case 1:
		dev = PCH_DEV_UART1;
		legacy = SIO_PCH_LEGACY_UART1;
		break;
	case 2:
		dev = PCH_DEV_UART2;
		legacy = SIO_PCH_LEGACY_UART2;
		break;
	default:
		return;
	}

	/* Set configured UART base address */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, (u32)base);

	/* Enable memory access and bus master */
	tmp = pci_read_config32(dev, PCI_COMMAND);
	tmp |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
	pci_write_config32(dev, PCI_COMMAND, tmp);

	/* Take UART out of reset */
	tmp = read32(base + SIO_REG_PPR_RESETS);
	tmp |= SIO_REG_PPR_RESETS_FUNC | SIO_REG_PPR_RESETS_APB |
		SIO_REG_PPR_RESETS_IDMA;
	write32(base + SIO_REG_PPR_RESETS, tmp);

	/* Set M and N divisor inputs and enable clock */
	tmp = read32(base + SIO_REG_PPR_CLOCK);
	tmp |= SIO_REG_PPR_CLOCK_EN | SIO_REG_PPR_CLOCK_UPDATE |
		(SIO_REG_PPR_CLOCK_N_DIV << 16) |
		(SIO_REG_PPR_CLOCK_M_DIV << 1);
	write32(base + SIO_REG_PPR_CLOCK, tmp);

	/* Put UART in byte access mode for 16550 compatibility */
	pcr_andthenor32(PID_SERIALIO, R_PCH_PCR_SERIAL_IO_GPPRVRW7, 0, legacy);
}
