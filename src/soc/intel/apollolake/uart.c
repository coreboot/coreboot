/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
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
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/uart.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>

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
