/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/* PCIe Ports */

Device (RP01)
{
	Name (_ADR, 0x00140000)
	Name (_DDN, "PCIe-B 0")

	#include "pcie_port.asl"
}

Device (RP03)
{
	Name (_ADR, 0x00130000)
	Name (_DDN, "PCIe-A 0")

	#include "pcie_port.asl"
}
