/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015-2017 Intel Corporation
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

#include <cbmem.h>
#include <console/uart.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <gpio.h>
#include <intelblocks/lpss.h>
#include <intelblocks/pcr.h>
#include <intelblocks/uart.h>
#include <soc/bootblock.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>

/* Serial IO UART controller legacy mode */
#define PCR_SERIAL_IO_GPPRVRW7		0x618
#define PCR_SIO_PCH_LEGACY_UART(idx)	(1 << (idx))

/* UART pad configuration. Support RXD and TXD for now. */
static const struct pad_config uart_pads[][2] = {
	{
		PAD_CFG_NF(GPP_C8, NONE, DEEP, NF1), /* UART0_RXD */
		PAD_CFG_NF(GPP_C9, NONE, DEEP, NF1), /* UART0_TXD */
	},
	{
		PAD_CFG_NF(GPP_C12, NONE, DEEP, NF1), /* UART1_RXD */
		PAD_CFG_NF(GPP_C13, NONE, DEEP, NF1), /* UART1_TXD */
	},
	{
		PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1), /* UART2_RXD */
		PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1), /* UART2_TXD */
	}
};

#if IS_ENABLED(CONFIG_DRIVERS_UART_8250MEM)
uintptr_t uart_platform_base(int idx)
{
	/* Same base address for all debug port usage. In reality UART2
	 * is currently only supported. */
	return UART_BASE_0_ADDR(idx);
}
#endif

void pch_uart_init(void)
{
	uintptr_t base = uart_platform_base(CONFIG_UART_FOR_CONSOLE);

	uart_common_init(pch_uart_get_debug_controller(), base);

	/* Put UART in byte access mode for 16550 compatibility */
	if (!IS_ENABLED(CONFIG_DRIVERS_UART_8250MEM_32)) {
		pcr_write32(PID_SERIALIO, PCR_SERIAL_IO_GPPRVRW7,
			PCR_SIO_PCH_LEGACY_UART(CONFIG_UART_FOR_CONSOLE));

		/*
		 * Dummy read after setting any of GPPRVRW7.
		 * Required for UART 16550 8-bit Legacy mode to become active
		 */
		lpss_clk_read(base);
	}

	gpio_configure_pads(uart_pads[CONFIG_UART_FOR_CONSOLE],
			    ARRAY_SIZE(uart_pads[CONFIG_UART_FOR_CONSOLE]));
}

#if !ENV_SMM
void pch_uart_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	/* Set the configured UART base address for the debug port */
	if (IS_ENABLED(CONFIG_UART_DEBUG) && uart_is_debug_controller(dev)) {
		struct resource *res = find_resource(dev, PCI_BASE_ADDRESS_0);
		/* Need to set the base and size for the resource allocator. */
		res->base = UART_BASE_0_ADDR(CONFIG_UART_FOR_CONSOLE);
		res->size = UART_DEBUG_BASE_0_SIZE;
		res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED |
			IORESOURCE_FIXED;
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
	switch (CONFIG_UART_FOR_CONSOLE) {
	case 0:
		return PCH_DEV_UART0;
	case 1:
		return PCH_DEV_UART1;
	case 2:
	default:
		return PCH_DEV_UART2;
	}
}
