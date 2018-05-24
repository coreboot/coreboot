/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Intel Corporation.
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
#include <assert.h>
#include <compiler.h>
#include <cbmem.h>
#include <console/uart.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <intelblocks/lpss.h>
#include <intelblocks/uart.h>
#include <soc/pci_devs.h>
#include <soc/iomap.h>
#include <soc/nvs.h>
#include <string.h>

#define UART_PCI_ENABLE	(PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER)
#define UART_CONSOLE_INVALID_INDEX	0xFF

extern const struct uart_gpio_pad_config uart_gpio_pads[];
extern const int uart_max_index;

static void uart_lpss_init(uintptr_t baseaddr)
{
	/* Take UART out of reset */
	lpss_reset_release(baseaddr);

	/* Set M and N divisor inputs and enable clock */
	lpss_clk_update(baseaddr, CONFIG_SOC_INTEL_COMMON_LPSS_UART_CLK_M_VAL,
			CONFIG_SOC_INTEL_COMMON_LPSS_UART_CLK_N_VAL);
}

#if IS_ENABLED(CONFIG_DRIVERS_UART_8250MEM)
uintptr_t uart_platform_base(int idx)
{
	/* return Base address for UART console index */
	return UART_BASE_0_ADDR(idx);
}
#endif

static int uart_get_valid_index(void)
{
	int index;

	for (index = 0; index < uart_max_index; index++) {
		if (uart_gpio_pads[index].console_index ==
				CONFIG_UART_FOR_CONSOLE)
			return index;
	}
	/* For valid index, code should not reach here */
	return UART_CONSOLE_INVALID_INDEX;
}

void uart_common_init(struct device *device, uintptr_t baseaddr)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = (pci_devfn_t)(uintptr_t)device;
#else
	struct device *dev = device;
#endif
	if (!dev)
		return;

	/* Set UART base address */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, baseaddr);

	/* Enable memory access and bus master */
	pci_write_config32(dev, PCI_COMMAND, UART_PCI_ENABLE);

	uart_lpss_init(baseaddr);
}

struct device *uart_get_device(void)
{
	/*
	 * This function will get called even if UART_DEBUG config options is
	 * not selected.
	 * By default returning NULL in case CONFIG_UART_DEBUG option is not
	 * selected to avoid compilation errors.
	 */
	if (!IS_ENABLED(CONFIG_UART_DEBUG))
		return NULL;

	int console_index = uart_get_valid_index();

	if (console_index != UART_CONSOLE_INVALID_INDEX)
		return soc_uart_console_to_device(CONFIG_UART_FOR_CONSOLE);
	else
		return NULL;
}

bool uart_is_controller_initialized(void)
{
	uintptr_t base;

#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = (pci_devfn_t)(uintptr_t)uart_get_device();
#else
	struct device *dev = uart_get_device();
#endif
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

static void uart_configure_gpio_pads(void)
{
	int index = uart_get_valid_index();

	if (index != UART_CONSOLE_INVALID_INDEX)
		gpio_configure_pads(uart_gpio_pads[index].gpios,
				MAX_GPIO_PAD_PER_UART);
}

void uart_bootblock_init(void)
{
	/* Program UART BAR0, command, reset and clock register */
	uart_common_init(uart_get_device(),
		UART_BASE(CONFIG_UART_FOR_CONSOLE));

	if (!IS_ENABLED(CONFIG_DRIVERS_UART_8250MEM_32))
		/* Put UART in byte access mode for 16550 compatibility */
		soc_uart_set_legacy_mode();

	/* Configure the 2 pads per UART. */
	uart_configure_gpio_pads();
}

#if ENV_RAMSTAGE

static void uart_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	/* Set the configured UART base address for the debug port */
	if (IS_ENABLED(CONFIG_UART_DEBUG) && uart_is_debug_controller(dev)) {
		struct resource *res = find_resource(dev, PCI_BASE_ADDRESS_0);
		/* Need to set the base and size for the resource allocator. */
		res->base = UART_BASE(CONFIG_UART_FOR_CONSOLE);
		res->size = UART_BASE_SIZE;
		res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED |
				IORESOURCE_FIXED;
	}
}

/*
 * Check if UART debug port controller needs to be initialized on resume.
 *
 * Returns:
 * true = when SoC wants debug port initialization on resume
 * false = otherwise
 */
static bool pch_uart_init_debug_controller_on_resume(void)
{
	global_nvs_t *gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);

	if (gnvs)
		return !!gnvs->uior;

	return false;
}

bool uart_is_debug_controller(struct device *dev)
{
	return dev == uart_get_device();
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
	 * check if SOC wants to initialize UART on resume
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
	.read_resources		= &uart_read_resources,
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
	.ops		= &device_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.devices	= pci_device_ids,
};
#endif /* ENV_RAMSTAGE */
