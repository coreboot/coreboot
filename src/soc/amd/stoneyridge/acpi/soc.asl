/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
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

Device(PCI0) {
	/* Describe the AMD Northbridge */
	#include "northbridge.asl"

	/* Describe the AMD Fusion Controller Hub */
	#include "sb_pci0_fch.asl"
}

/* Describe PCI INT[A-H] for the Southbridge */
#include "pci_int.asl"

/* Describe the devices in the Southbridge */
#include "sb_fch.asl"

/* Add GPIO library */
#include <gpio_lib.asl>
