/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/uart.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>

#include "uart8250mem.h"

uintptr_t uart_platform_base(unsigned int idx)
{
	uint32_t reg32 = pci_read_config32(PCH_DEV_UART(idx), UART_MEMBA);

	reg32 &= ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;

	return reg32;
}

unsigned int uart_platform_refclk(void)
{
	unsigned int ret = 115200 * 16;

	/**
	 * Base uart clock is HIGH_SPEED_CLK_MULT (24) * 1.8432Mhz if using baudrates > 115200.
	 */
	if (CONFIG_TTYS0_BAUD > 115200)
		ret *= HIGH_SPEED_CLK_MULT;

	return ret;
}
