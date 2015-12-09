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
 */

#include <stddef.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/pmc.h>
#include <soc/romstage.h>
#include "../chip.h"

void tco_disable(void)
{
	uint32_t reg;

	reg = inl(ACPI_BASE_ADDRESS + TCO1_CNT);
	reg |= TCO_TMR_HALT;
	outl(reg, ACPI_BASE_ADDRESS + TCO1_CNT);
}
