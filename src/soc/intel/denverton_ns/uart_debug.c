/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdint.h>
#include <console/uart.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <soc/uart.h>

#define MY_PCI_DEV(SEGBUS, DEV, FN) \
	((((SEGBUS)&0xFFF) << 20) | (((DEV)&0x1F) << 15) | (((FN)&0x07) << 12))

uintptr_t uart_platform_base(unsigned int idx)
{
	return (uintptr_t)pci_io_read_config32(
		       MY_PCI_DEV(0, CONFIG_HSUART_DEV, idx),
		       PCI_BASE_ADDRESS_1) +
	       SIZE_OF_HSUART_RES * idx;
}
