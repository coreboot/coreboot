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

#include <stdint.h>
#include <arch/io.h>
#include <arch/early_variables.h>
#include <delay.h>
#include <uart8250.h>
#include <device/pci_def.h>

#define PCIE_BRIDGE \
	PCI_DEV(CONFIG_OXFORD_OXPCIE_BRIDGE_BUS, \
		CONFIG_OXFORD_OXPCIE_BRIDGE_DEVICE, \
		CONFIG_OXFORD_OXPCIE_BRIDGE_FUNCTION)

#define OXPCIE_DEVICE \
	PCI_DEV(CONFIG_OXFORD_OXPCIE_BRIDGE_SUBORDINATE, 0, 0)

#define OXPCIE_DEVICE_3 \
	PCI_DEV(CONFIG_OXFORD_OXPCIE_BRIDGE_SUBORDINATE, 0, 3)

#if defined(__PRE_RAM__)
int oxford_oxpcie_present CAR_GLOBAL;

void oxford_init(void)
{
	u16 reg16;
	oxford_oxpcie_present = 1;

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
		oxford_oxpcie_present = 0;
		return;
	}

	/* Setup base address on device */
	pci_write_config32(device, PCI_BASE_ADDRESS_0,
				CONFIG_OXFORD_OXPCIE_BASE_ADDRESS);

	/* Enable memory on device */
	reg16 = pci_read_config16(device, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MEMORY;
	pci_write_config16(device, PCI_COMMAND, reg16);

	/* Now the UART initialization */
	u32 uart0_base = CONFIG_OXFORD_OXPCIE_BASE_ADDRESS + 0x1000;

	uart8250_mem_init(uart0_base, (4000000 / CONFIG_TTYS0_BAUD));
}

#endif
