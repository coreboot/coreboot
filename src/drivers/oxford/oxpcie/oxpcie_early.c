/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#define __SIMPLE_DEVICE__

#include <stdint.h>
#include <stddef.h>
#include <arch/io.h>
#include <arch/early_variables.h>
#include <delay.h>
#include <console/uart.h>
#include <device/pci_def.h>

static unsigned int oxpcie_present CAR_GLOBAL;
static ROMSTAGE_CONST u32 uart0_base = CONFIG_OXFORD_OXPCIE_BASE_ADDRESS + 0x1000;
static ROMSTAGE_CONST u32 uart1_base = CONFIG_OXFORD_OXPCIE_BASE_ADDRESS + 0x2000;

#define PCIE_BRIDGE \
	PCI_DEV(CONFIG_OXFORD_OXPCIE_BRIDGE_BUS, \
		CONFIG_OXFORD_OXPCIE_BRIDGE_DEVICE, \
		CONFIG_OXFORD_OXPCIE_BRIDGE_FUNCTION)

#define OXPCIE_DEVICE \
	PCI_DEV(CONFIG_OXFORD_OXPCIE_BRIDGE_SUBORDINATE, 0, 0)

#define OXPCIE_DEVICE_3 \
	PCI_DEV(CONFIG_OXFORD_OXPCIE_BRIDGE_SUBORDINATE, 0, 3)

static void oxpcie_init_bridge(void)
{
	u16 reg16;

	/* First we reset the secondary bus */
	reg16 = pci_read_config16(PCIE_BRIDGE, PCI_BRIDGE_CONTROL);
	reg16 |= (1 << 6); /* SRESET */
	pci_write_config16(PCIE_BRIDGE, PCI_BRIDGE_CONTROL, reg16);

	/* Assume we don't have to wait here forever */

	/* Read back and clear reset bit. */
	reg16 = pci_read_config16(PCIE_BRIDGE, PCI_BRIDGE_CONTROL);
	reg16 &= ~(1 << 6); /* SRESET */
	pci_write_config16(PCIE_BRIDGE, PCI_BRIDGE_CONTROL, reg16);

	/* Set up subordinate bus number */
	pci_write_config8(PCIE_BRIDGE, PCI_SECONDARY_BUS, 0x00);
	pci_write_config8(PCIE_BRIDGE, PCI_SUBORDINATE_BUS, 0x00);
	pci_write_config8(PCIE_BRIDGE, PCI_SECONDARY_BUS,
			CONFIG_OXFORD_OXPCIE_BRIDGE_SUBORDINATE);
	pci_write_config8(PCIE_BRIDGE, PCI_SUBORDINATE_BUS,
			CONFIG_OXFORD_OXPCIE_BRIDGE_SUBORDINATE);

	/* Memory window for the OXPCIe952 card */
	// XXX is the calculation of base and limit correct?
	pci_write_config32(PCIE_BRIDGE, PCI_MEMORY_BASE,
			((CONFIG_OXFORD_OXPCIE_BASE_ADDRESS & 0xffff0000) |
			((CONFIG_OXFORD_OXPCIE_BASE_ADDRESS >> 16) & 0xff00)));

	/* Enable memory access through bridge */
	reg16 = pci_read_config16(PCIE_BRIDGE, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MEMORY;
	pci_write_config16(PCIE_BRIDGE, PCI_COMMAND, reg16);

	u32 timeout = 20000; // Timeout in 10s of microseconds.
	u32 id = 0;
	for (;;) {
		id = pci_read_config32(OXPCIE_DEVICE, PCI_VENDOR_ID);
		if (!timeout-- || (id != 0 && id != 0xffffffff))
			break;
		udelay(10);
	}

	u32 device = OXPCIE_DEVICE; /* unknown default */
	switch (id) {
	case 0xc1181415: /* e.g. Startech PEX1S1PMINI */
		/* On this device function 0 is the parallel port, and
		 * function 3 is the serial port. So let's go look for
		 * the UART.
		 */
		id = pci_read_config32(OXPCIE_DEVICE_3, PCI_VENDOR_ID);
		if (id != 0xc11b1415)
			return;
		device = OXPCIE_DEVICE_3;
		break;
	case 0xc1581415: /* e.g. Startech MPEX2S952 */
		device = OXPCIE_DEVICE;
		break;
	default:
		/* No UART here. */
		return;
	}

	/* Setup base address on device */
	pci_write_config32(device, PCI_BASE_ADDRESS_0,
				CONFIG_OXFORD_OXPCIE_BASE_ADDRESS);

	/* Enable memory on device */
	reg16 = pci_read_config16(device, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MEMORY;
	pci_write_config16(device, PCI_COMMAND, reg16);

	car_set_var(oxpcie_present, 1);
}

static int oxpcie_uart_active(void)
{
	return (car_get_var(oxpcie_present));
}

unsigned int uart_platform_base(int idx)
{
	if (idx == 0 && oxpcie_uart_active())
		return uart0_base;
	if (idx == 1 && oxpcie_uart_active())
		return uart1_base;
	return 0;
}

#ifndef __PRE_RAM__
void oxford_remap(u32 new_base)
{
	uart0_base = new_base + 0x1000;
	uart1_base = new_base + 0x2000;
}

uint32_t uartmem_getbaseaddr(void)
{
	return uart_platform_base(0);
}
#endif

unsigned int uart_platform_refclk(void)
{
	return 62500000;
}

void oxford_init(void)
{
	oxpcie_init_bridge();
	uart_init();
}
