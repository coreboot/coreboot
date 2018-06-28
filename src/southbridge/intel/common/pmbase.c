/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Patrick Rudolph <patrick.rudolph@9elements.com>
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

#include <stdint.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/early_variables.h>
#include <assert.h>

#include "pmbase.h"

/* LPC PM Base Address Register */
#define PMBASE		0x40
#define PMSIZE		0x80

/* PCI Configuration Space (D31:F0): LPC */
#if defined(__SIMPLE_DEVICE__)
#define PCH_LPC_DEV	PCI_DEV(0, 0x1f, 0)
#else
#define PCH_LPC_DEV	dev_find_slot(0, PCI_DEVFN(0x1f, 0))
#endif

u16 lpc_get_pmbase(void)
{
#if defined(__SMM__)
	/* Don't assume PMBASE is still the same */
	return pci_read_config16(PCH_LPC_DEV, PMBASE) & 0xfffc;
#else
	static u16 pmbase CAR_GLOBAL;

	if (pmbase)
		return pmbase;

	pmbase = pci_read_config16(PCH_LPC_DEV, PMBASE) & 0xfffc;

	return pmbase;
#endif
}

void write_pmbase32(const u8 addr, const u32 val)
{
	ASSERT(addr <= (PMSIZE - sizeof(u32)));

	outl(val, lpc_get_pmbase() + addr);
}

void write_pmbase16(const u8 addr, const u16 val)
{
	ASSERT(addr <= (PMSIZE - sizeof(u16)));

	outw(val, lpc_get_pmbase() + addr);
}

void write_pmbase8(const u8 addr, const u8 val)
{
	ASSERT(addr <= (PMSIZE - sizeof(u8)));

	outb(val, lpc_get_pmbase() + addr);
}

u32 read_pmbase32(const u8 addr)
{
	ASSERT(addr <= (PMSIZE - sizeof(u32)));

	return inl(lpc_get_pmbase() + addr);
}

u16 read_pmbase16(const u8 addr)
{
	ASSERT(addr <= (PMSIZE - sizeof(u16)));

	return inw(lpc_get_pmbase() + addr);
}

u8 read_pmbase8(const u8 addr)
{
	ASSERT(addr <= (PMSIZE - sizeof(u8)));

	return inb(lpc_get_pmbase() + addr);
}
