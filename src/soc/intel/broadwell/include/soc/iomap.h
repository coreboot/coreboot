/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _BROADWELL_IOMAP_H_
#define _BROADWELL_IOMAP_H_

#define MCFG_BASE_ADDRESS	CONFIG_MMCONF_BASE_ADDRESS
#define MCFG_BASE_SIZE		0x4000000

#define MCH_BASE_ADDRESS	0xfed10000
#define MCH_BASE_SIZE		0x8000

#define DMI_BASE_ADDRESS	0xfed18000
#define DMI_BASE_SIZE		0x1000

#define EP_BASE_ADDRESS		0xfed19000
#define EP_BASE_SIZE		0x1000

#define EDRAM_BASE_ADDRESS	0xfed80000
#define EDRAM_BASE_SIZE		0x4000

#define GDXC_BASE_ADDRESS	0xfed84000
#define GDXC_BASE_SIZE		0x1000

#define RCBA_BASE_ADDRESS	0xfed1c000
#define RCBA_BASE_SIZE		0x4000

#define HPET_BASE_ADDRESS	0xfed00000

#define GFXVT_BASE_ADDRESS	0xfed90000ULL
#define GFXVT_BASE_SIZE		0x1000

#define VTVC0_BASE_ADDRESS	0xfed91000ULL
#define VTVC0_BASE_SIZE		0x1000

#define ACPI_BASE_ADDRESS	0x1000
#define ACPI_BASE_SIZE		0x100

#define GPIO_BASE_ADDRESS	0x1400
#define GPIO_BASE_SIZE		0x400

#define SMBUS_BASE_ADDRESS	0x0400
#define SMBUS_BASE_SIZE		0x10

/* Temporary addresses used in romstage */
#define EARLY_GTT_BAR		0xe0000000
#define EARLY_XHCI_BAR		0xd7000000
#define EARLY_EHCI_BAR		CONFIG_EHCI_BAR
#define EARLY_UART_BAR		CONFIG_TTYS0_BASE
#define EARLY_TEMP_MMIO		0xfed08000

#endif
