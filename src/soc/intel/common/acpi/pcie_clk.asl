/* SPDX-License-Identifier: GPL-2.0-or-later */

/* PCH clock by P2SB */
#include <soc/intel/common/acpi/pch_clk.asl>

/* IOE clock by P2SB */
#include <soc/intel/common/acpi/ioe_clk.asl>

/*
 * CLKM (Clock Manager): Helper Method to manage clock enable/disable
 * This method handles the enabling or disabling of clocks for either the
 * Integrated Controller (ICLK) or the IOE Die (ECLK) based on the provided
 * parameters.
 *
 * Arg0: Clock number
 * Arg1: Clock source, IOE Die Clock (1)/Integrated Controller Clock (0)
 * Arg2: Enable(1)/Disable(0) Clock
 */
Method (CLKM, 3, Serialized) {
	If (LEqual (Arg1, 1)) {
		If (LEqual (Arg2, 1)) {
			\_SB.ECLK.CLKE (Arg0)
		} Else {
			\_SB.ECLK.CLKD (Arg0)
		}
	} Else {
		If (LEqual (Arg2, 1)) {
			\_SB.ICLK.CLKE (Arg0)
		} Else {
			\_SB.ICLK.CLKD (Arg0)
		}
	}
}

/*
 * Configure PCIe ClkReq Override
 * Arg0: Clock number
 * Arg1: Enable(1)/Disable(0) Clock
 */
Method (SPCO, 2, Serialized) {
	/* Flag to indicate presence of IOE Die (1 = Present, 0 = Not Present) */
	Local0 = CONFIG(SOC_INTEL_COMMON_BLOCK_IOE_P2SB)
	/* Clock start index */
	Local1 = 0

	/* Override clock start index if SOC_INTEL_COMMON_BLOCK_IOE_P2SB Kconfig is present. */
	If (LEqual (Local0, 1)) {
		Local1 = CONFIG_IOE_DIE_CLOCK_START
	}

	/* Clock number */
	Local2 = Arg0
	If (LGreaterEqual (Arg0, Local1)) {
		Local2 = Subtract (Arg0, Local1)
	} Else {
		/*
		 * Override IOE die indicator if clock number is less than the IOE die clock
		 * start index. Refer as clock number from non-IOE die.
		 */
		Local0 = 0;
	}

	CLKM (Local2, Local0, Arg1)
}
