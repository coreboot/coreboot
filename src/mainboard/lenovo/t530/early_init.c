/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Vladimir Serbinenko
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
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <drivers/lenovo/hybrid_graphics/hybrid_graphics.h>
#include <device/device.h>

static void hybrid_graphics_init(void)
{
	bool peg, igd;
	u32 reg32;

	early_hybrid_graphics(&igd, &peg);

	if (peg && igd)
		return;

	/* Hide disabled devices */
	reg32 = pci_read_config32(PCI_DEV(0, 0, 0), DEVEN);
	reg32 &= ~(DEVEN_PEG10 | DEVEN_IGD);

	if (peg)
		reg32 |= DEVEN_PEG10;

	if (igd)
		reg32 |= DEVEN_IGD;
	else
		/* Disable IGD VGA decode, no GTT or GFX stolen */
		pci_write_config16(PCI_DEV(0, 0, 0), GGC, 2);

	pci_write_config32(PCI_DEV(0, 0, 0), DEVEN, reg32);
}

void mainboard_early_init(int s3resume)
{
	hybrid_graphics_init();
}
