/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This defines the battery charging thresholds setting methods tpacpi-bat can
 * use. This implements what the vendor defines but is rather ugly...
 */

/* SetBatteryCharge Start/Stop Capacity Threshold
 * In Parameter:
 * DWORD
 * Bit 7-0: Charge stop capacity (Unit:%)
 *     =0: Use battery default setting
 *     =1-99: Threshold to stop charging battery (Relative capacity)
 * Bit 9-8:BatteryID
 *    = 0: Any battery
 *    = 1: Primary battery
 *    = 2: Secondary battery
 *    = Others: Reserved (0)
 * Bit 31-10: Reserved (0)
 *     Must be 0
 *
 * Out Parameter:
 * DWORD
 * Bit 30-0: Reserved (0)
 * Bit 31:     Error status
 *  0 ... Success
 *  1 ... Fail
 */

#define START_THRESH_ARG 0
#define STOP_THRESH_ARG 1

// Set stop threshold
Method (BCSS, 1, NotSerialized)
{
	Local0 = Arg0 & 0xff        // Percentage
	Local1 = (Arg0 >> 8) & 0x3  // Battery ID

	// Any battery
	If (Local1 == 0)
	{
		\_SB.PCI0.LPCB.EC.BAT0.SETT(STOP_THRESH_ARG, Local0)
		\_SB.PCI0.LPCB.EC.BAT1.SETT(STOP_THRESH_ARG, Local0)

		Local2 = Local0 != \_SB.PCI0.LPCB.EC.BAT0.GETT(STOP_THRESH_ARG)
		Local3 = Local0 != \_SB.PCI0.LPCB.EC.BAT1.GETT(STOP_THRESH_ARG)

		Return ((Local2 && Local3) << 31)
	}

	// Battery1
	If (Local1 == 1)
	{
		\_SB.PCI0.LPCB.EC.BAT0.SETT(STOP_THRESH_ARG, Local0)
		Return ((Local0 !=
			\_SB.PCI0.LPCB.EC.BAT0.GETT(STOP_THRESH_ARG)) << 31)
	}

	// Battery2
	If (Local1 == 2)
	{
		\_SB.PCI0.LPCB.EC.BAT1.SETT(STOP_THRESH_ARG, Local0)
		Return ((Local0 !=
			\_SB.PCI0.LPCB.EC.BAT1.GETT(STOP_THRESH_ARG)) << 31)
	}

	Return (1 << 31) /* Should not be reached */
}

// Set start threshold
Method (BCCS, 1, NotSerialized)
{
	Local0 = Arg0 & 0xff        // Percentage
	Local1 = (Arg0 >> 8) & 0x3  // Battery ID

	// Any battery
	If (Local1 == 0)
	{
		\_SB.PCI0.LPCB.EC.BAT0.SETT(START_THRESH_ARG, Local0)
		\_SB.PCI0.LPCB.EC.BAT1.SETT(START_THRESH_ARG, Local0)

		Local2 = Local0 != \_SB.PCI0.LPCB.EC.BAT0.GETT(START_THRESH_ARG)
		Local3 = Local0 != \_SB.PCI0.LPCB.EC.BAT1.GETT(START_THRESH_ARG)

		Return ((Local2 && Local3) << 31)
	}

	// Battery1
	If (Local1 == 1)
	{
		\_SB.PCI0.LPCB.EC.BAT0.SETT(START_THRESH_ARG, Local0)
		Return ((Local0 !=
			\_SB.PCI0.LPCB.EC.BAT0.GETT(START_THRESH_ARG)) << 31)
	}

	// Battery2
	If (Local1 == 2)
	{
		\_SB.PCI0.LPCB.EC.BAT1.SETT(START_THRESH_ARG, Local0)
		Return ((Local0 !=
			\_SB.PCI0.LPCB.EC.BAT1.GETT(START_THRESH_ARG)) << 31)
	}

	Return (1 << 31) /* Should not be reached */
}

/*
 * GetBatteryCharge Start/Stop Capacity Threshold
 * In Parameter:
 * DWORD
 * Bit 7-0:BatteryID
 * Bit 31-8: Reserved (0)
 *     Must be 0
 *
 * Out Parameter:
 * DWORD
 * Bit 7-0: Charge stop capacity (Unit:%)
 *     =0: Use battery default setting
 *     =1-99: Threshold to stop charging battery (Relative capacity)
 *     =Others: Reserved (0)
 * Bit 9-8: Capability of BatteryCharge Stop Capacity Threshold
 * Bit 8:Batterycharge stop capacity threshold
 *     (0:Not support   1:Support)
 * Bit 9: Specify every battery parameter
 *     (0:Not support(apply parameter for all battery)
 *      1:Support(apply parameter for all battery))
 * Bit 30-10: Reserved (0)
 * Bit 31:     Error status
 *     0 ... Success
 *     1 ... Fail
*/

// Get stop threshold
Method (BCSG, 1, NotSerialized)
{
	// Battery1
	If (Arg0 == 1)
	{
		Return (0x300 | \_SB.PCI0.LPCB.EC.BAT0.GETT(STOP_THRESH_ARG))
	}

	// Battery2
	If (Arg0 == 2)
	{
		Return (0x300 | \_SB.PCI0.LPCB.EC.BAT1.GETT(STOP_THRESH_ARG))
	}

	Return (1 << 31)
}

// Get start threshold
Method (BCTG, 1, NotSerialized)
{
	// Battery 1
	If (Arg0 == 1)
	{
		Return (0x300 | \_SB.PCI0.LPCB.EC.BAT0.GETT(START_THRESH_ARG))
	}

	// Battery 2
	If (Arg0 == 2)
	{
		Return (0x300 | \_SB.PCI0.LPCB.EC.BAT1.GETT(START_THRESH_ARG))
	}

	Return (1 << 31)
}
