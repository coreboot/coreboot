/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

// Intel LPC Bus Device  - 0:1f.0
#include <soc/intel/common/block/acpi/acpi/lpc.asl>

Scope (\_SB.PCI0.LPCB)
{
	#include <acpi/ec.asl>
	#include <acpi/superio.asl>
}
