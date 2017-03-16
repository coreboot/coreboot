/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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

/* IPU3 input system - Device 05, Function 0 */
Device (IMGU)
{
	Name (_ADR, 0x00050000)
	Name (_DDN, "Imaging Unit")
	Name (_CCA, ZERO)
	Name (CAMD, 0x01)
}

/* IPU3 processing system - Device 14, Function 3 */
Device (CIO2)
{
	Name (_ADR, 0x00140003)
	Name (_DDN, "Camera and Imaging Subsystem")
	Name (_CCA, ZERO)
	Name (CAMD, 0x00)
}
