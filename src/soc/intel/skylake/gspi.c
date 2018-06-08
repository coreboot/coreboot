/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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
#include <intelblocks/gspi.h>
#include <soc/pci_devs.h>

int gspi_soc_bus_to_devfn(unsigned int gspi_bus)
{
	switch (gspi_bus) {
	case 0:
		return PCH_DEVFN_GSPI0;
	case 1:
		return PCH_DEVFN_GSPI1;
	}
	return -1;
}
