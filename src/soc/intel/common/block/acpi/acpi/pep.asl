/* SPDX-License-Identifier: GPL-2.0-only */

#define LPID_DSM_ARG2_ENUM_FUNCTIONS 0
#define LPID_DSM_ARG2_GET_DEVICE_CONSTRAINTS 1

#define LPID_DSM_ARG2_GET_CRASH_DUMP_DEV 2
#define LPID_DSM_ARG2_DISPLAY_OFF_NOTIFY 3
#define LPID_DSM_ARG2_DISPLAY_ON_NOTIFY 4
#define LPID_DSM_ARG2_S0IX_ENTRY 5
#define LPID_DSM_ARG2_S0IX_EXIT 6

External(\_SB.MS0X, MethodObj)
External(\_SB.PCI0.LPCB.EC0.S0IX, MethodObj)
External(\_SB.PCI0.EGPM, MethodObj)
External(\_SB.PCI0.RGPM, MethodObj)

Device(LPID)
{
	Name(_ADR, 0x00000000)
	Name(_CID, EISAID("PNP0D80"))
	Name(UUID, ToUUID("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66"))
	Method(_DSM, 4)
	{
		If(Arg0 == ^UUID) {
			/*
			 * Enum functions
			 */
			If(Arg2 == LPID_DSM_ARG2_ENUM_FUNCTIONS) {
				Return(Buffer(One) {0x60})
			}
			/*
			 * Function 1 - Get Device Constraints
			 */
			If(Arg2 == LPID_DSM_ARG2_GET_DEVICE_CONSTRAINTS) {
				Return(Package(5) {0, Ones, Ones, Ones, Ones})
			}
			/*
			 * Function 2 - Get Crash Dump Device
			 */
			If(Arg2 == LPID_DSM_ARG2_GET_CRASH_DUMP_DEV) {
				Return(Buffer(One) {0x0})
			}
			/*
			 * Function 3 - Display Off Notification
			 */
			If(Arg2 == LPID_DSM_ARG2_DISPLAY_OFF_NOTIFY) {
			}
			/*
			 * Function 4 - Display On Notification
			 */
			If(Arg2 == LPID_DSM_ARG2_DISPLAY_ON_NOTIFY) {
			}
			/*
			 * Function 5 - Low Power S0 Entry Notification
			 */
			If(Arg2 == LPID_DSM_ARG2_S0IX_ENTRY) {
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
			If(Arg2 == LPID_DSM_ARG2_S0IX_EXIT) {
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
}			// Device (LPID)
