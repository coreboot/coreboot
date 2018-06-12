/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2017 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * The sole purpose of this driver is to avoid BAR to be changed during
 * resource allocation. Since configuration space is just 32 bytes it
 * shouldn't cause any fragmentation.
 */

#include <cbmem.h>
#include <console/uart.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/uart.h>
#include <soc/gpio.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <soc/uart.h>

static const struct pad_config uart_gpios[] = {
#if IS_ENABLED(CONFIG_SOC_INTEL_GLK)
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_60, NATIVE, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPSS_UART0_RXD */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_61, NATIVE, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPSS_UART0_TXD */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_64, NATIVE, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPSS_UART2_RXD */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_65, NATIVE, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPSS_UART2_TXD */
#else
	PAD_CFG_NF(GPIO_42, NATIVE, DEEP, NF1),		/* UART1 RX */
	PAD_CFG_NF(GPIO_43, NATIVE, DEEP, NF1),		/* UART1 TX */
	PAD_CFG_NF(GPIO_46, NATIVE, DEEP, NF1),		/* UART2 RX */
	PAD_CFG_NF(GPIO_47, NATIVE, DEEP, NF1),		/* UART2 TX */
#endif
};

static inline int invalid_uart_for_console(void)
{
	/* There are actually only 2 UARTS, and they are named UART1 and
	 * UART2. They live at pci functions 1 and 2 respectively. */
	if (CONFIG_UART_FOR_CONSOLE > 2 || CONFIG_UART_FOR_CONSOLE < 1)
		return 1;
	return 0;
}

void pch_uart_init(void)
{
	uintptr_t base = CONFIG_CONSOLE_UART_BASE_ADDRESS;
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t uart = _PCH_DEV(UART, CONFIG_UART_FOR_CONSOLE & 3);
#else
	struct device *uart = _PCH_DEV(UART, CONFIG_UART_FOR_CONSOLE & 3);
#endif

	/* Get a 0-based pad index. See invalid_uart_for_console() above. */
	const int pad_index = CONFIG_UART_FOR_CONSOLE - 1;

	if (invalid_uart_for_console())
		return;

	/* Configure the 2 pads per UART. */
	gpio_configure_pads(&uart_gpios[pad_index * 2], 2);

	/* Program UART2 BAR0, command, reset and clock register */
	uart_common_init(uart, base);

}

#if !ENV_SMM
void pch_uart_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	if (IS_ENABLED(CONFIG_SOC_UART_DEBUG) &&
		uart_is_debug_controller(dev)) {
		/* will override existing resource. */
		fixed_mem_resource(dev, PCI_BASE_ADDRESS_0,
				CONFIG_CONSOLE_UART_BASE_ADDRESS >> 10, 4, 0);
	}
}
#endif

bool pch_uart_init_debug_controller_on_resume(void)
{
	global_nvs_t *gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);

	if (gnvs)
		return !!gnvs->uior;

	return false;
}

device_t pch_uart_get_debug_controller(void)
{
	return _PCH_DEV(UART, CONFIG_UART_FOR_CONSOLE);
}

uintptr_t uart_platform_base(int idx)
{
	return CONFIG_CONSOLE_UART_BASE_ADDRESS;
}
