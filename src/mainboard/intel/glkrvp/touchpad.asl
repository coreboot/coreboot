/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
	Scope (\_SB.PCI0.I2C4)
	{
		Device (TPAD)
		{
		Name(_ADR, One)
		Name (_HID, "ALPS0001")
		Name (_CID, "PNP0C50")
		Name (_DDN, "ALPS Touchpad")
		Name (_UID, 1)
		Name (ISTP, 1) /* Touchpad */
		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x2C,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C4",        // ResourceSource
			)
			Interrupt (ResourceConsumer, Level, ActiveLow)
			{
				GPIO_18_IRQ
			}
			GpioInt (Level, ActiveLow, ExclusiveAndWake, PullUp, 0x0000, "\\_SB.GPO1", 0x00, ResourceConsumer, ,)
			{
				18
			}
		})

		Method (_STA)
		{
				Return (0xF)
		}
		Method(_DSM, 0x4, NotSerialized)
		{
		// DSM UUID for HIDI2C - HID driver does not load without DSM
			If(LEqual(Arg0, ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE")))
			{
				// Function 0 : Query Function
				If(LEqual(Arg2, Zero))
				{
					// Revision 1
					If(LEqual(Arg1, One))
					{
						Return (Buffer (One) {0x03})
					}
					Else
					{
						Return (Buffer (One) {0x00})
					}
				} ElseIf (LEqual(Arg2, One)) {  // Function 1 : HID Function
			// HID Descriptor Address (IHV Specific)
					Return(0x0020)
				} Else {
					Return (Buffer (One) {0x00})
				}
			} Else {
				Return (Buffer (One) {0x00})
			}
		}
	}
}
