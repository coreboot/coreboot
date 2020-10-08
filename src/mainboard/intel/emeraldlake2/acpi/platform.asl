/* SPDX-License-Identifier: GPL-2.0-only */

/* The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method(_PTS,1)
{
	// NVS has a flag to determine USB policy in S3
	if (S3U0) {
		GP47 = 1   // Enable USB0
	} Else {
		GP47 = 0  // Disable USB0
	}

	// NVS has a flag to determine USB policy in S3
	if (S3U1) {
		GP56 = 1   // Enable USB1
	} Else {
		GP56 = 0  // Disable USB1
	}
}

/* The _WAK method is called on system wakeup */

Method(_WAK,1)
{
	Return(Package(){0,0})
}
