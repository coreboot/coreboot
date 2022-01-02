/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/common/acpi/platform.asl>

/* The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method(_PTS,1)
{
}

/* The _WAK method is called on system wakeup */

Method(_WAK,1)
{
	/* Update AC status */
	Store (\_SB.PCI0.LPCB.EC0.ACEX, Local0)
	if (Local0 != \PWRS) {
		Store (Local0, \PWRS)
		Notify (\_SB.PCI0.LPCB.EC0.AC, 0x80)
	}

	/* Update LID status */
	Store (\_SB.PCI0.LPCB.EC0.LIDS, Local0)
	if (Local0 != \LIDS) {
		Store (Local0, \LIDS)
		Notify (\_SB.PCI0.LPCB.EC0.LID0, 0x80)
	}

	Return(Package(){0,0})
}
