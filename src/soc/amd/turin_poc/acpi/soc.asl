/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/amd/common/acpi/cxl_root.asl>
#include "globalnvs.asl"

Scope(\_SB) {
	/* global utility methods expected within the \_SB scope */
	#include <arch/x86/acpi/globutil.asl>

	#include <soc/amd/common/acpi/gpio_bank_lib.asl>

	#include <soc/amd/common/acpi/osc.asl>

	#include "pci_int_defs.asl"

	#include "mmio.asl"

	CXL_ROOT_BRIDGE(S0B0, 0)
	CXL_ROOT_BRIDGE(S0B1, 1)
	CXL_ROOT_BRIDGE(S0B2, 2)
	CXL_ROOT_BRIDGE(S0B3, 3)
	CXL_ROOT_BRIDGE(S0B4, 4)
	CXL_ROOT_BRIDGE(S0B5, 5)
	CXL_ROOT_BRIDGE(S0B6, 6)
	CXL_ROOT_BRIDGE(S0B7, 7)

	#include "cxl_root_dev.asl"

	Scope(S0B0) {
		#include "resources.asl"
		#include <soc/amd/common/acpi/lpc.asl>
	} /* End S0B0 scope */

	#include "ioapic_routing.asl"
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
}
