/* SPDX-License-Identifier: GPL-2.0-only */

#define DPTF_CPU_PASSIVE	98
#define DPTF_CPU_CRITICAL	125
#define DPTF_CPU_ACTIVE_AC0	91
#define DPTF_CPU_ACTIVE_AC1	85
#define DPTF_CPU_ACTIVE_AC2	83
#define DPTF_CPU_ACTIVE_AC3	80
#define DPTF_CPU_ACTIVE_AC4	75

Name (DTRT, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.B0D4, \_SB.PCI0.B0D4, 100, 50, 0, 0, 0, 0 },

})

Name (MPPC, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		10000,	/* PowerLimitMinimum */
		31000,	/* PowerLimitMaximum */
		28000,	/* TimeWindowMinimum */
		28000,	/* TimeWindowMaximum */
		100	/* StepSize */
	},
	Package () {	/* Power Limit 2 */
		1,	/* PowerLimitIndex, 1 for Power Limit 2 */
		15000,	/* PowerLimitMinimum */
		65000,	/* PowerLimitMaximum */
		28000,	/* TimeWindowMinimum */
		28000,	/* TimeWindowMaximum */
		100	/* StepSize */
	}
})

/* Include DPTF */
#include <soc/intel/skylake/acpi/dptf/dptf.asl>
