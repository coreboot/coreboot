/* SPDX-License-Identifier: GPL-2.0-only */

/* The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method(_PTS,1)
{
	GP35 = 0  // Disable WLAN
	GP38 = 0  // Disable WWAN

	If (S33G) {
		GP43 = 0  // Enable HSPA
	} Else {
		GP43 = 1   // Disable HSPA
	}

	If (Arg0 == 3) {
		// NVS has a flag to determine USB policy in S3
		If (S3U0) {
			GP47 = 1   // Enable USB0
		} Else {
			GP47 = 0  // Disable USB0
		}

		// NVS has a flag to determine USB policy in S3
		If (S3U1) {
			GP56 = 1   // Enable USB1
		} Else {
			GP56 = 0  // Disable USB1
		}
	}
	If (Arg0 == 5) {
		// NVS has a flag to determine USB policy in S5
		If (S5U0) {
			GP47 = 1   // Enable USB0
		} Else {
			GP47 = 0  // Disable USB0
		}

		// NVS has a flag to determine USB policy in S5
		If (S5U1) {
			GP56 = 1   // Enable USB1
		} Else {
			GP56 = 0  // Disable USB1
		}
	}
}

/* The _WAK method is called on system wakeup */

Method(_WAK,1)
{
	/* Update in case state changed while asleep */
	\PWRS = \_SB.PCI0.LPCB.EC0.ACEX

	/* Enable OS control of fan */
	\_SB.PCI0.LPCB.EC0.FCOS = 1

	Return(Package(){0,0})
}
