/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include "pch.h"
#include "cpu/intel/model_206ax/model_206ax.h"
#include <cpu/x86/msr.h>

static void write8p(uintptr_t addr, uint32_t val)
{
	write8((u8 *)addr, val);
}

static void write16p(uintptr_t addr, uint32_t val)
{
	write16((u16 *)addr, val);
}

static uint16_t read16p (uintptr_t addr)
{
	return read16((u16 *)addr);
}

/* Early thermal init, must be done prior to giving ME its memory
   which is done at the end of raminit.  */
void early_thermal_init(void)
{
	pci_devfn_t dev;
	msr_t msr;

	dev = PCI_DEV(0x0, 0x1f, 0x6);

	/* Program address for temporary BAR.  */
	pci_write_config32(dev, 0x40, 0x40000000);
	pci_write_config32(dev, 0x44, 0x0);

	/* Activate temporary BAR.  */
	pci_write_config32(dev, 0x40,
			   pci_read_config32(dev, 0x40) | 5);


	write16p (0x40000004, 0x3a2b);
	write8p (0x4000000c, 0xff);
	write8p (0x4000000d, 0x00);
	write8p (0x4000000e, 0x40);
	write8p (0x40000082, 0x00);
	write8p (0x40000001, 0xba);

	/* Perform init.  */
	/* Configure TJmax.  */
	msr = rdmsr(MSR_TEMPERATURE_TARGET);
	write16p(0x40000012, ((msr.lo >> 16) & 0xff) << 6);
	/* Northbridge temperature slope and offset.  */
	write16p(0x40000016, 0x808c);

	write16p (0x40000014, 0xde87);

	/* Enable thermal data reporting, processor, PCH and northbridge.  */
	write16p(0x4000001a, (read16p(0x4000001a) & ~0xf) | 0x10f0);

	/* Disable temporary BAR.  */
	pci_write_config32(dev, 0x40,
			   pci_read_config32(dev, 0x40) & ~1);
	pci_write_config32(dev, 0x40, 0);

	write32 (DEFAULT_RCBA + 0x38b0,
		 (read32 (DEFAULT_RCBA + 0x38b0) & 0xffff8003) | 0x403c);
}
