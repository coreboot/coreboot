/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <soc/intel/common/block/acpi/acpi/ipu.asl>

/* IPU3 processing system - Device 14, Function 3 */
Device (CIO2)
{
	Name (_ADR, 0x00140003)
	Name (_DDN, "Camera and Imaging Subsystem")
	Name (CAMD, 0x00)
}
