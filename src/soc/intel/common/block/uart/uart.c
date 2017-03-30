/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/pci_def.h>
#include <intelblocks/lpss.h>
#include <intelblocks/uart.h>

void uart_common_init(device_t dev, uintptr_t baseaddr, uint32_t clk_m_val,
		uint32_t clk_n_val)
{
	/* Set UART base address */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, baseaddr);

	/* Enable memory access and bus master */
	pci_write_config32(dev, PCI_COMMAND,
			PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

	/* Take UART out of reset */
	lpss_reset_release(baseaddr);

	/* Set M and N divisor inputs and enable clock */
	lpss_clk_update(baseaddr, clk_m_val, clk_n_val);
}
