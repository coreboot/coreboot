/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Device: Intel's IO Self-Testing Software (IOST)
 * NOTE: The IOSE Name object is used to enable IOST. It is set to 0 by default,
 * and its value can be updated in the SSDT.
 */
Device (IOST)
{
	//
	// Intel IO Self Testing Device
	//
	Name (_UID, "IOST")
	Name (_HID, "IOS0001")
	Name (IVER, 0x00010000)
	Name (IOSE, 0x00)
	Method (_STA, 0, NotSerialized)
	{
		If (IOSE > Zero) {
			Return (0xf)
		}
		Return (Zero)
	}

	/*
	 * _DSM Device Specific Method
	 *
	 * Arg0: UUID : 893f00a6-660c-494e-bcfd-3043f4fb67c0
	 * Arg1: Integer Revision Level
	 * Arg2: Integer Function Index (0 = Return Supported Functions)
	 * Arg3: Package Parameters
	 */
	Method (_DSM, 4, Serialized)
	{
		If ( (IOSE > Zero) && LEqual (Arg0, ToUUID ("893f00a6-660c-494e-bcfd-3043f4fb67c0")))
		{
			If (LEqual (0, ToInteger (Arg1)))
			{
				Switch (ToInteger (Arg2))
				{
					/* Function 0: returns a buffer containing one bit for each function index, starting with zero.
					 * Bit 0 indicates whether there is support for any function other than function 0
					 */
					Case (0)
					{
						Return (Buffer(){0x07})
					}
					/* Function 1: P2SB Read
					 * Package Details
					 * Idx0 = PortID
					 * Idx1 = Offset
					 */
					Case (1)
					{
						Local0 = DeRefOf (Index (Arg3, 0)) /* PordID */
						Local1 = DeRefOf (Index (Arg3, 1)) /* offset */

						Local3 = PCRR(Local0, Local1)
						Printf ("[IOST] P2B Read: PortID: %o  Offset: %o read = %o", ToHexString(Local0), ToHexString(Local1), ToHexString(Local3))
						Return(Local3)
					}
					/* Function 2: P2SB Write
					 * Package Details
					 * Idx0 = PortID
					 * Idx1 = Offset
					 * Idx2 = DataToWrite
					 */
					Case (2)
					{
						Local0 = DeRefOf (Index (Arg3, 0)) /* PordID */
						Local1 = DeRefOf (Index (Arg3, 1)) /* offset */
						Local2 = DeRefOf (Index (Arg3, 2)) /* data to Write */
						Printf ("[IOST] P2B Write: PortID: %o Offset: %o with %o", ToHexString(Local0), ToHexString(Local1), ToHexString(Local2))
						PCRW(Local0, Local1, Local2)
						Return (0x00)
					}
				}
			}
		}
		Return (Buffer () {0x00})
	}
}
