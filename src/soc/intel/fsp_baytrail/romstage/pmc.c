/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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
 * Foundation, Inc.
 */

#include <stddef.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <baytrail/iomap.h>
#include <baytrail/iosf.h>
#include <baytrail/lpc.h>
#include <baytrail/pci_devs.h>
#include <baytrail/pmc.h>
#include <baytrail/romstage.h>
#include "../chip.h"

void tco_disable(void)
{
	uint32_t reg;

	reg = inl(ACPI_BASE_ADDRESS + TCO1_CNT);
	reg |= TCO_TMR_HALT;
	outl(reg, ACPI_BASE_ADDRESS + TCO1_CNT);
}
