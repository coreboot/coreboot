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
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
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

#if ENV_RAMSTAGE

__attribute__((weak)) void pch_uart_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);
}

static struct device_operations device_ops = {
	.read_resources		= &pch_uart_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources		= &pci_dev_enable_resources,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_SPT_UART0,
	PCI_DEVICE_ID_INTEL_SPT_UART1,
	PCI_DEVICE_ID_INTEL_SPT_UART2,
	PCI_DEVICE_ID_INTEL_KBP_H_UART0,
	PCI_DEVICE_ID_INTEL_KBP_H_UART1,
	PCI_DEVICE_ID_INTEL_KBP_H_UART2,
	PCI_DEVICE_ID_INTEL_APL_UART0,
	PCI_DEVICE_ID_INTEL_APL_UART1,
	PCI_DEVICE_ID_INTEL_APL_UART2,
	PCI_DEVICE_ID_INTEL_APL_UART3,
	PCI_DEVICE_ID_INTEL_GLK_UART0,
	PCI_DEVICE_ID_INTEL_GLK_UART1,
	PCI_DEVICE_ID_INTEL_GLK_UART2,
	PCI_DEVICE_ID_INTEL_GLK_UART3,
	0,
};

static const struct pci_driver pch_uart __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.devices	= pci_device_ids,
};
#endif /* ENV_RAMSTAGE */
