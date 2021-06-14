/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_IOMAP_H_
#define _BROADWELL_IOMAP_H_

#include <northbridge/intel/haswell/memmap.h>

#define HPET_BASE_ADDRESS	0xfed00000

#define ACPI_BASE_ADDRESS	0x1000
#define ACPI_BASE_SIZE		0x100

#define GPIO_BASE_ADDRESS	0x1400
#define GPIO_BASE_SIZE		0x400

/* Temporary addresses used in romstage */
#define EARLY_GTT_BAR		0xe0000000
#define EARLY_XHCI_BAR		0xd7000000
#define EARLY_EHCI_BAR		CONFIG_EHCI_BAR
#define EARLY_UART_BAR		CONFIG_TTYS0_BASE
#define EARLY_TEMP_MMIO		0xfed08000

#endif
