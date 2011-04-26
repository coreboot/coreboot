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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdint.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <uart8250.h>
#include <device/pci_def.h>

#define PCIE_BRIDGE \
	PCI_DEV(CONFIG_OXFORD_OXPCIE_BRIDGE_BUS, \
		CONFIG_OXFORD_OXPCIE_BRIDGE_DEVICE, \
		CONFIG_OXFORD_OXPCIE_BRIDGE_FUNCTION)

#define OXPCIE_DEVICE \
	PCI_DEV(CONFIG_OXFORD_OXPCIE_BRIDGE_SUBORDINATE, 0, 0) 

void oxford_init(void)
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
	// XXX is the calculation of base and limit corect?
	pci_write_config32(PCIE_BRIDGE, PCI_MEMORY_BASE, 
			((CONFIG_OXFORD_OXPCIE_BASE_ADDRESS & 0xffff0000) |
			((CONFIG_OXFORD_OXPCIE_BASE_ADDRESS >> 16) & 0xff00)));

	/* Enable memory access through bridge */
	reg16 = pci_read_config16(PCIE_BRIDGE, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MEMORY;
	pci_write_config16(PCIE_BRIDGE, PCI_COMMAND, reg16);

	// FIXME Add a timeout or this will hang forever if 
	// no device is in the slot.
	u32 id = 0;
	while ((id == 0) || (id == 0xffffffff))
		id = pci_read_config32(OXPCIE_DEVICE, PCI_VENDOR_ID);

	/* Setup base address on device */
	pci_write_config32(OXPCIE_DEVICE, PCI_BASE_ADDRESS_0,
				CONFIG_OXFORD_OXPCIE_BASE_ADDRESS);

	/* Enable memory on device */
	reg16 = pci_read_config16(OXPCIE_DEVICE, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MEMORY;
	pci_write_config16(OXPCIE_DEVICE, PCI_COMMAND, reg16);

	/* Now the UART initialization */
	u32 uart0_base = CONFIG_OXFORD_OXPCIE_BASE_ADDRESS + 0x1000;

	uart8250_mem_init(uart0_base, (4000000 / CONFIG_TTYS0_BAUD));
}

