/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#include <agesawrapper.h>
#include <assert.h>
#include <stdint.h>
#include <arch/io.h>
#include <arch/acpi.h>
#include <console/console.h>
#include <reset.h>
#include <arch/cpu.h>
#include <cbmem.h>
#include <soc/southbridge.h>
#include <soc/pci_devs.h>
#include <cpu/x86/msr.h>
#include <delay.h>

static void enable_wideio(uint8_t port, uint16_t size)
{
	uint32_t wideio_enable[] = {
		LPC_WIDEIO0_ENABLE,
		LPC_WIDEIO1_ENABLE,
		LPC_WIDEIO2_ENABLE
	};
	uint32_t alt_wideio_enable[] = {
		LPC_ALT_WIDEIO0_ENABLE,
		LPC_ALT_WIDEIO1_ENABLE,
		LPC_ALT_WIDEIO2_ENABLE
	};
	pci_devfn_t dev = PCI_DEV(0, PCU_DEV, LPC_FUNC);
	uint32_t tmp;

	/* Only allow port 0-2 */
	assert(port <= ARRAY_SIZE(wideio_enable));

	if (size == 16) {
		tmp = pci_read_config32(dev, LPC_ALT_WIDEIO_RANGE_ENABLE);
		tmp |= alt_wideio_enable[port];
		pci_write_config32(dev, LPC_ALT_WIDEIO_RANGE_ENABLE, tmp);
	} else { 	/* 512 */
		tmp = pci_read_config32(dev, LPC_ALT_WIDEIO_RANGE_ENABLE);
		tmp &= ~alt_wideio_enable[port];
		pci_write_config32(dev, LPC_ALT_WIDEIO_RANGE_ENABLE, tmp);
	}

	/* Enable the range */
	tmp = pci_read_config32(dev, LPC_IO_OR_MEM_DECODE_ENABLE);
	tmp |= wideio_enable[port];
	pci_write_config32(dev, LPC_IO_OR_MEM_DECODE_ENABLE, tmp);
}

/*
 * lpc_wideio_window() may be called any point in romstage, but take
 * care that AGESA doesn't overwrite the range this function used.
 * The function checks if there is an empty range and if all ranges are
 * used the function throws an assert. The function doesn't check for a
 * duplicate range, for ranges that can  be merged into a single
 * range, or ranges that overlap.
 *
 * The developer is expected to ensure that there are no conflicts.
 */
static void lpc_wideio_window(uint16_t base, uint16_t size)
{
	pci_devfn_t dev = PCI_DEV(0, PCU_DEV, LPC_FUNC);
	u32 tmp;

	/* Support 512 or 16 bytes per range */
	assert(size == 512 || size == 16);

	/* Find and open Base Register and program it */
	tmp = pci_read_config32(dev, LPC_WIDEIO_GENERIC_PORT);

	if ((tmp & 0xffff) == 0) {	/* WIDEIO0 */
		tmp |= base;
		pci_write_config32(dev, LPC_WIDEIO_GENERIC_PORT, tmp);
		enable_wideio(0, size);
	} else if ((tmp & 0xffff0000) == 0) {	/* WIDEIO1 */
		tmp |= (base << 16);
		pci_write_config32(dev, LPC_WIDEIO_GENERIC_PORT, tmp);
		enable_wideio(1, size);
	} else { /* Check WIDEIO2 register */
		tmp = pci_read_config32(dev, LPC_WIDEIO2_GENERIC_PORT);
		if ((tmp & 0xffff) == 0) {	/* WIDEIO2 */
			tmp |= base;
			pci_write_config32(dev, LPC_WIDEIO2_GENERIC_PORT, tmp);
			enable_wideio(2, size);
		} else {	/* All WIDEIO locations used*/
			assert(0);
		}
	}
}

void lpc_wideio_512_window(uint16_t base)
{
	assert(IS_ALIGNED(base, 512));
	lpc_wideio_window(base, 512);
}

void lpc_wideio_16_window(uint16_t base)
{
	assert(IS_ALIGNED(base, 16));
	lpc_wideio_window(base, 16);
}
