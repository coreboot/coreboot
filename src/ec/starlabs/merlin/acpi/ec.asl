/* SPDX-License-Identifier: GPL-2.0-only */

#include "ubtc.asl"

Scope (\_SB.PCI0.LPCB)
{
	#include "cmos.asl"

	Device (EC)
	{
		Name (_HID, EisaId ("PNP0C09"))
		Name (_UID, 0x01)
		Name (_GPE, CONFIG_EC_GPE_SCI)
		Name (ECAV, 0x00)
		Name (ECTK, 0x01)
		Name (B2ST, 0x00)
		Name (CFAN, 0x00)
		Name (CMDR, 0x00)
		Name (DOCK, 0x00)
		Name (PLMX, 0x00)
		Name (PECH, 0x00)
		Name (PECL, 0x00)
		Name (PENV, 0x00)
		Name (PINV, 0x00)
		Name (PPSH, 0x00)
		Name (PPSL, 0x00)
		Name (PSTP, 0x00)
		Name (RPWR, 0x00)
		Name (VPWR, 0x00)
		Name (WTMS, 0x00)
		Name (AWT2, 0x00)
		Name (AWT1, 0x00)
		Name (AWT0, 0x00)
		Name (DLED, 0x00)
		Name (SPT2, 0x00)
		Name (PB10, 0x00)
		Name (IWCW, 0x00)
		Name (IWCR, 0x00)
		Name (PVOL, 0x00)
		Mutex (ECMT, 0x00)

		Name (BFFR, ResourceTemplate()
		{
			IO(Decode16, 0x0062, 0x0062, 0x00, 0x01)
			IO(Decode16, 0x0066, 0x0066, 0x00, 0x01)
		})

		Method (_CRS, 0, Serialized)
		{
			Return (BFFR)
		}

		Method (_STA, 0, NotSerialized)
		{
			\LIDS = 0x03
			Return (0x0F)
		}

		OperationRegion (SIPR, SystemIO, 0xB2, 0x1)
		Field (SIPR, ByteAcc, Lock, Preserve) {
			SMB2, 8
		}

		#include "emem.asl"

		// ECRD (Embedded Controller Read Method)
		//
		// Handle all commands sent to EC by BIOS
		//
		// Arguments:
		// Arg0 = Object to Read
		//
		// Return Value:
		// Read Value
		//
		Method (ECRD, 1, Serialized, 0, IntObj, FieldUnitObj)
		{
			//
			// Check for ECDT support, set ECAV to 1 if ECDT is supported by OS
			// Only check once at beginning since ECAV might be clear later in certain conditions
			//
			If (ECTK) {
				If (_REV >= 0x02) {
					ECAV = 0x01
				}
				ECTK = 0x00					// Clear flag for checking once only
			}

			Local0 = Acquire (ECMT, 1000)				// Save Acquired Result
			If (Local0 == 0x00)					// Check for Mutex Acquisition
			{
				If (ECAV) {
					Local1 = DerefOf (Arg0)			// Execute Read from EC
					Release (ECMT)
					Return (Local1)
				} Else {
					Release (ECMT)
				}
			}
			Return (0)						// Return in case Arg0 doesn't exist
		}

		// ECWR (Embedded Controller Write Method)
		//
		// Handle all commands sent to EC by BIOS
		//
		// Arguments:
		// Arg0 = Value to Write
		// Arg1 = Object to Write to
		//
		// Return Value:
		// None
		//
		Method (ECWR, 2, Serialized,,,{IntObj, FieldUnitObj})
		{
			Local0 = Acquire (ECMT, 1000)				// Save Acquired Result
			If (Local0 == 0x00)					// Check for Mutex Acquisition
			{
				If (ECAV) {
					Arg1 = Arg0				// Execute Write to EC
					Local1 = 0x00
					While (1) {
						If (Arg0 == DerefOf (Arg1)) {
							Break
						}
						Sleep (1)
						Arg1 = Arg0
						Local1 += 1
						If (Local1 == 0x03) {
							Break
						}
					}
				}
				Release (ECMT)
			}
		}

		#include "ac.asl"
		#include "battery.asl"
		#include "events.asl"
		#include "lid.asl"
		#include "typec.asl"

		Method (_REG, 2, NotSerialized)
		{
			If ((Arg0 == 0x03) && (Arg1 == 0x01))
			{
				// Load EC Driver
				ECAV = 0x01

				// Initialise the Lid State
				\LIDS = LSTE

				// Initialise the OS State
				OSFG = 0x01

				// Initialise the Power State
				PWRS = (ECRD (RefOf(ECPS)) & 0x01)

				// Inform the platform code
				PNOT()
			}
		}
	}
}
