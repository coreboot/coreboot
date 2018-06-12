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

#include <arch/acpi.h>
#include <compiler.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <intelblocks/lpss.h>
#include <intelblocks/uart.h>

#define UART_PCI_ENABLE	(PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER)

static void uart_lpss_init(uintptr_t baseaddr)
{
	/* Take UART out of reset */
	lpss_reset_release(baseaddr);

	/* Set M and N divisor inputs and enable clock */
	lpss_clk_update(baseaddr, CONFIG_SOC_INTEL_COMMON_LPSS_UART_CLK_M_VAL,
			CONFIG_SOC_INTEL_COMMON_LPSS_UART_CLK_N_VAL);
}

void uart_common_init(device_t dev, uintptr_t baseaddr)
{
	/* Set UART base address */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, baseaddr);

	/* Enable memory access and bus master */
	pci_write_config32(dev, PCI_COMMAND, UART_PCI_ENABLE);

	uart_lpss_init(baseaddr);
}

__weak device_t pch_uart_get_debug_controller(void)
{
	/*
	 * device_t can either be a pointer to struct device (e.g. ramstage) or
	 * a simple integer (e.g. SMM) depending upon whether __SIMPLE_DEVICE__
	 * is defined for the stage. Thus, the return requires additional
	 * casting to uintptr_t.
	 */
	return (device_t)(uintptr_t)NULL;
}

bool uart_debug_controller_is_initialized(void)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev;
#else
	struct device *dev;
#endif
	uintptr_t base;

	dev = pch_uart_get_debug_controller();
	if (!dev)
		return false;

	base = pci_read_config32(dev, PCI_BASE_ADDRESS_0) & ~0xFFF;
	if (!base)
		return false;

	if ((pci_read_config32(dev, PCI_COMMAND) & UART_PCI_ENABLE)
	    != UART_PCI_ENABLE)
		return false;

	return !lpss_is_controller_in_reset(base);
}

#if ENV_RAMSTAGE

__weak void pch_uart_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);
}

__weak bool pch_uart_init_debug_controller_on_resume(void)
{
	/* By default, do not initialize controller. */
	return false;
}

bool uart_is_debug_controller(struct device *dev)
{
	return dev == pch_uart_get_debug_controller();
}

/*
 * This is a workaround to enable UART controller for the debug port if:
 * 1. CONSOLE_SERIAL is not enabled in coreboot, and
 * 2. This boot is S3 resume, and
 * 3. SoC wants to initialize debug UART controller.
 *
 * This workaround is required because Linux kernel hangs on resume if console
 * is not enabled in coreboot, but it is enabled in kernel and not suspended.
 */
static bool uart_controller_needs_init(struct device *dev)
{
	/*
	 * If coreboot has CONSOLE_SERIAL enabled, the skip re-initializing
	 * controller here.
	 */
	if (IS_ENABLED(CONFIG_CONSOLE_SERIAL))
		return false;

	/* If this device does not correspond to debug port, then skip. */
	if (!uart_is_debug_controller(dev))
		return false;

	/* Initialize UART controller only on S3 resume. */
	if (!acpi_is_wakeup_s3())
		return false;

	/*
	 * Call SoC specific routine to confirm it wants to initialize
	 * controller.
	 */
	return pch_uart_init_debug_controller_on_resume();
}

static void uart_common_enable_resources(struct device *dev)
{
	pci_dev_enable_resources(dev);

	if (uart_controller_needs_init(dev)) {
		uintptr_t base;

		base = pci_read_config32(dev, PCI_BASE_ADDRESS_0) & ~0xFFF;
		if (base)
			uart_lpss_init(base);
	}
}

static struct device_operations device_ops = {
	.read_resources		= &pch_uart_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &uart_common_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_SPT_UART0,
	PCI_DEVICE_ID_INTEL_SPT_UART1,
	PCI_DEVICE_ID_INTEL_SPT_UART2,
	PCI_DEVICE_ID_INTEL_SPT_H_UART0,
	PCI_DEVICE_ID_INTEL_SPT_H_UART1,
	PCI_DEVICE_ID_INTEL_SPT_H_UART2,
	PCI_DEVICE_ID_INTEL_KBP_H_UART0,
	PCI_DEVICE_ID_INTEL_KBP_H_UART1,
	PCI_DEVICE_ID_INTEL_KBP_H_UART2,
	PCI_DEVICE_ID_INTEL_APL_UART0,
	PCI_DEVICE_ID_INTEL_APL_UART1,
	PCI_DEVICE_ID_INTEL_APL_UART2,
	PCI_DEVICE_ID_INTEL_APL_UART3,
	PCI_DEVICE_ID_INTEL_CNL_UART0,
	PCI_DEVICE_ID_INTEL_CNL_UART1,
	PCI_DEVICE_ID_INTEL_CNL_UART2,
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
