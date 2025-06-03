/* SPDX-License-Identifier: GPL-2.0-or-later */

/* PCH clock by P2SB */
#include <soc/intel/common/acpi/pch_clk.asl>

/* IOE clock by P2SB */
#if CONFIG(SOC_INTEL_COMMON_BLOCK_IOE_P2SB)
	#include <soc/intel/common/acpi/ioe_clk.asl>
#endif

/*
 * Configure PCIe ClkReq Override
 * Arg0: Clock number
 * Arg1: Enable(1)/Disable(0) Clock
 */
Method (SPCO, 2, Serialized) {
#if CONFIG(SOC_INTEL_COMMON_BLOCK_IOE_P2SB)
	If (LEqual (Arg1,1)) {
		If (LGreaterEqual (Arg0, CONFIG_IOE_DIE_CLOCK_START)) {
			\_SB.ECLK.CLKE (Subtract (Arg0, CONFIG_IOE_DIE_CLOCK_START))
		} Else {
			\_SB.ICLK.CLKE (Arg0)
		}
	} Else {
		If (LGreaterEqual (Arg0, CONFIG_IOE_DIE_CLOCK_START)) {
			\_SB.ECLK.CLKD (Subtract (Arg0, CONFIG_IOE_DIE_CLOCK_START))
		} Else {
			\_SB.ICLK.CLKD (Arg0)
		}
	}
#else
	If (LEqual (Arg1,1)) {
		\_SB.ICLK.CLKE (Arg0)
	} Else {
		\_SB.ICLK.CLKD (Arg0)
	}
#endif
}
