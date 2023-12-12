/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/amd/common/acpi/pci_root.asl>
#include "globalnvs.asl"

Scope(\_SB) {
	/* global utility methods expected within the \_SB scope */
	#include <arch/x86/acpi/globutil.asl>

	#include <soc/amd/common/acpi/gpio_bank_lib.asl>

	#include <soc/amd/common/acpi/osc.asl>

	#include "pci_int_defs.asl"

	#include "mmio.asl"

	ROOT_BRIDGE(S0B0)
	ROOT_BRIDGE(S0B1)
	ROOT_BRIDGE(S0B2)
	ROOT_BRIDGE(S0B3)
} /* End \_SB scope */

#include <soc/amd/common/acpi/alib.asl>

#include <soc/amd/common/acpi/platform.asl>

#include <soc/amd/common/acpi/sleepstates.asl>

/*
 * Platform Notify
 *
 * This is called by soc/amd/common/acpi/platform.asl.
 */
Method (PNOT)
{
	/* Report AC/DC state to ALIB using WAL1() */
	\WAL1 ()
}
