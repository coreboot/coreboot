/* SPDX-License-Identifier: GPL-2.0-only */

#define PEPD_DSM_LPI_ENUM_FUNCTIONS 0
#define PEPD_DSM_LPI_GET_DEVICE_CONSTRAINTS 1

#define PEPD_DSM_LPI_GET_CRASH_DUMP_DEV 2
#define PEPD_DSM_LPI_DISPLAY_OFF_NOTIFY 3
#define PEPD_DSM_LPI_DISPLAY_ON_NOTIFY 4
#define PEPD_DSM_LPI_S0IX_ENTRY 5
#define PEPD_DSM_LPI_S0IX_EXIT 6

External(\_SB.MS0X, MethodObj)
External(\_SB.PCI0.LPCB.EC0.S0IX, MethodObj)
External(\_SB.PCI0.EGPM, MethodObj)
External(\_SB.PCI0.RGPM, MethodObj)

Device(PEPD)
{
	Name(_ADR, 0x00000000)
	Name(_CID, EISAID("PNP0D80"))
	Method(_DSM, 4)
	{
		/* Low Power Idle S0 helper */
		If(Arg0 == ToUUID("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66")) {
			/*
			 * Enum functions
			 */
			If(Arg2 == PEPD_DSM_LPI_ENUM_FUNCTIONS) {
				Return(Buffer(One) {0x60})
			}
			/*
			 * Function 1 - Get Device Constraints
			 *
			 * Device constraints for low power states (may be used for debugging).
			 * For now there is only one disabled dummy device, because Windows
			 * expects at least one device and crashes without it with a bluescreen
			 * (`INTERNAL_POWER_ERROR`). Returning an empty package does not work.
			 */
			If(Arg2 == PEPD_DSM_LPI_GET_DEVICE_CONSTRAINTS) {
				Return (Package() {
						Package() {
							"\\DUMY",		/* \DUMY - not existent */
							0,			/* disabled, no constraints */
							Package() {
								0,		/* revision */
								Package() {
									0xff,	/* apply to all LPIT states */
									0	/* minimum D-state */
								}
							}
						}
					}
				)
			}
			/*
			 * Function 2 - Get Crash Dump Device
			 */
			If(Arg2 == PEPD_DSM_LPI_GET_CRASH_DUMP_DEV) {
			}
			/*
			 * Function 3 - Display Off Notification
			 */
			If(Arg2 == PEPD_DSM_LPI_DISPLAY_OFF_NOTIFY) {
			}
			/*
			 * Function 4 - Display On Notification
			 */
			If(Arg2 == PEPD_DSM_LPI_DISPLAY_ON_NOTIFY) {
			}
			/*
			 * Function 5 - Low Power S0 Entry Notification
			 */
			If(Arg2 == PEPD_DSM_LPI_S0IX_ENTRY) {
				/* Inform the EC */
				If (CondRefOf (\_SB.PCI0.LPCB.EC0.S0IX)) {
					\_SB.PCI0.LPCB.EC0.S0IX(1)
				}

				/* provide board level S0ix hook */
				If (CondRefOf (\_SB.MS0X)) {
					\_SB.MS0X(1)
				}

				/*
				 * Save the current PM bits then
				 * enable GPIO PM with MISCCFG_ENABLE_GPIO_PM_CONFIG
				 */
				If (CondRefOf (\_SB.PCI0.EGPM))
				{
					\_SB.PCI0.EGPM ()
				}
			}
			/*
			 * Function 6 - Low Power S0 Exit Notification
			 */
			If(Arg2 == PEPD_DSM_LPI_S0IX_EXIT) {
				/* Inform the EC */
				If (CondRefOf (\_SB.PCI0.LPCB.EC0.S0IX)) {
					\_SB.PCI0.LPCB.EC0.S0IX(0)
				}

				/* provide board level S0ix hook */
				If (CondRefOf (\_SB.MS0X)) {
					\_SB.MS0X(0)
				}

				/* Restore GPIO all Community PM */
				If (CondRefOf (\_SB.PCI0.RGPM))
				{
					\_SB.PCI0.RGPM ()
				}
			}
		}

		Return(Buffer(One) {0x00})
	}		// Method(_DSM)
}			// Device (PEPD)
