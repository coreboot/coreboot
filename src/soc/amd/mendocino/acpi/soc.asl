/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#include "globalnvs.asl"

Scope(\_SB) {
	/* global utility methods expected within the \_SB scope */
	#include <arch/x86/acpi/globutil.asl>

	#include <soc/amd/common/acpi/gpio_bank_lib.asl>

	#include <soc/amd/common/acpi/osc.asl>

	#include "pci_int_defs.asl"

	#include <soc/amd/common/acpi/pci_int.asl>

	#include "mmio.asl"

	#include "pci0.asl"
} /* End \_SB scope */

#include <soc/amd/common/acpi/alib.asl>

#include <soc/amd/common/acpi/platform.asl>

#include <soc/amd/common/acpi/sleepstates.asl>

#include <soc/amd/common/acpi/upep.asl>

#if CONFIG(SOC_AMD_COMMON_BLOCK_ACPI_DPTC)
#include <soc/amd/common/acpi/dptc.asl>
#endif

#include "rtc_workaround.asl"

/* Enable DPTC interface with AMD ALIB */
External(\_SB.DPTC, MethodObj)

/*
 * Platform Notify
 *
 * This is called by soc/amd/common/acpi/platform.asl.
 */
Method (PNOT)
{
	/* Report AC/DC state to ALIB using WAL1() */
	\WAL1 ()

	If (CondRefOf (\_SB.DPTC)) {
		\_SB.DPTC()
	}
}
