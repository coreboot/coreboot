/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

External(\_SB.MS0X, MethodObj)
External(\_SB.PCI0.LPCB.EC0.S0IX, MethodObj)
External(\_SB.PCI0.EGPM, MethodObj)
External(\_SB.PCI0.RGPM, MethodObj)

scope(\_SB)
{
	Device(LPID) {
	Name(_ADR, 0x00000000)
	Name(_CID, EISAID("PNP0D80"))
	Name(UUID,
		ToUUID("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66"))
		Method(_DSM, 4) {
			If(Arg0 == ^UUID) {
			/*
			 * Enum functions
			 */
			If(Arg2 == Zero) {
				Return(Buffer(One) {
				       0x60}
				)
			}
			/*
			 * Function 1 - Get Device Constraints
			 */
			If(Arg2 == 1) {
				Return(Package(5) {
				       0, Ones, Ones, Ones, Ones}
				)
			}
			/*
			 * Function 2 - Get Crash Dump Device
			 */
			If(Arg2 == 2) {
				Return(Buffer(One) {
				       0x0}
				)
			}
			/*
			 * Function 3 - Display Off Notification
			 */
			If(Arg2 == 3) {
			}
			/*
			 * Function 4 - Display On Notification
			 */
			If(Arg2 == 4) {
			}
			/*
			 * Function 5 - Low Power S0 Entry Notification
			 */
			If(Arg2 == 5) {
				/* Inform the EC */
				If (CondRefOf (\_SB.PCI0.LPCB.EC0.S0IX)) {
					\_SB.PCI0.LPCB.EC0.S0IX(1)
				}

				/* provide board level s0ix hook */
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
			If(Arg2 == 6) {
				/* Inform the EC */
				If (CondRefOf (\_SB.PCI0.LPCB.EC0.S0IX)) {
					\_SB.PCI0.LPCB.EC0.S0IX(0)
				}

				/* provide board level s0ix hook */
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
	}			// device (LPID)
}				// End Scope(\_SB)
