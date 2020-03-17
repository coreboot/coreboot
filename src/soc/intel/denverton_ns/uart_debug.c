/*
 * This file is part of the coreboot project.
 *
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

#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <soc/uart.h>

#define MY_PCI_DEV(SEGBUS, DEV, FN) \
	((((SEGBUS)&0xFFF) << 20) | (((DEV)&0x1F) << 15) | (((FN)&0x07) << 12))

uintptr_t uart_platform_base(int idx);

uintptr_t uart_platform_base(int idx)
{
	return (uintptr_t)pci_io_read_config32(
		       MY_PCI_DEV(0, CONFIG_HSUART_DEV, idx),
		       PCI_BASE_ADDRESS_1) +
	       SIZE_OF_HSUART_RES * idx;
}
