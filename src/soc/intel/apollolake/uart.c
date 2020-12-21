/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * The sole purpose of this driver is to avoid BAR to be changed during
 * resource allocation. Since configuration space is just 32 bytes it
 * shouldn't cause any fragmentation.
 */

#include <commonlib/helpers.h>
#include <device/pci_type.h>
#include <soc/pci_devs.h>

const unsigned int uart_devices[] = {
	PCH_DEVFN_UART0,
#if CONFIG(SOC_INTEL_GEMINILAKE)
	PCI_DEVFN_INVALID,
#else
	PCH_DEVFN_UART1,
#endif
	PCH_DEVFN_UART2,
};

const int uart_devices_size = ARRAY_SIZE(uart_devices);
