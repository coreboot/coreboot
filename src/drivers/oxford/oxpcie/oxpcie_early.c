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
#include <console/uart.h>
#include <device/pci_def.h>

static unsigned int oxpcie_present CAR_GLOBAL;
static ROMSTAGE_CONST u32 uart0_base = CONFIG_EARLY_PCI_MMIO_BASE + 0x1000;
static ROMSTAGE_CONST u32 uart1_base = CONFIG_EARLY_PCI_MMIO_BASE + 0x2000;

static int oxpcie_probe(u8 bus, u8 dev, u32 mmio_base, u16 io_base)
{
	pci_devfn_t device = PCI_DEV(bus, dev, 0);

	u32 id = pci_read_config32(device, PCI_VENDOR_ID);
	switch (id) {
	case 0xc1181415: /* e.g. Startech PEX1S1PMINI function 0 */
		/* On this device function 0 is the parallel port, and
		 * function 3 is the serial port. So let's go look for
		 * the UART.
		 */
		device = PCI_DEV(bus, dev, 3);
		id = pci_read_config32(device, PCI_VENDOR_ID);
		if (id != 0xc11b1415)
			return -1;
		break;
	case 0xc11b1415: /* e.g. Startech PEX1S1PMINI function 3 */
	case 0xc1581415: /* e.g. Startech MPEX2S952 */
		break;
	default:
		/* No UART here. */
		return -1;
	}

	/* Setup base address on device */
	pci_write_config32(device, PCI_BASE_ADDRESS_0, mmio_base);

	/* Enable memory on device */
	u16 reg16 = pci_read_config16(device, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MEMORY;
	pci_write_config16(device, PCI_COMMAND, reg16);

	car_set_var(oxpcie_present, 1);
	return 0;
}

static int oxpcie_uart_active(void)
{
	return (car_get_var(oxpcie_present));
}

#ifndef __PRE_RAM__
void oxford_remap(u32 new_base)
{
	uart0_base = new_base + 0x1000;
	uart1_base = new_base + 0x2000;
}
#endif

unsigned uart_platform_base(int idx)
{
	if (idx == 0 && oxpcie_uart_active())
		return uart0_base;
	if (idx == 1 && oxpcie_uart_active())
		return uart1_base;
	return 0;
}

unsigned uart_platform_divisor(void)
{
	return uart_divisor(4000000);
}

uint32_t uartmem_getbaseaddr(void)
{
	return uart_platform_base(0);
}

void oxford_init(void)
{
	pci_bridge_early_init(&oxpcie_probe);
	uart_init();
}
