/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
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

#include <variant/gpio.h>

#define BOARD_DIG_I2C_ADDR			0x09
#define BOARD_DIG_IRQ				DIG_INT_L
#define BOARD_DIG_PDCT				DIG_PDCT_L
#define BOARD_DIG_EJECT				GPE_DIG_EJECT

Scope (\_SB)
{
	Device (PENH)
	{
		Name (_HID, "PRP0001")

		Name (_CRS, ResourceTemplate () {
			GpioIo (Exclusive, PullNone, 0, 0, IoRestrictionInputOnly,
				"\\_SB.PCI0.GPIO", 0, ResourceConsumer) { GPIO_DIG_EJECT }
		})

		Name (_DSD, Package () {
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () {
					"compatible",
					Package () { "gpio-keys"}
				},
			}
		})

		Device (EJCT)
		{
			Name (_ADR, Zero)

			Name (_DSD, Package () {
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package () {
					/* SW_PEN_INSERTED */
					Package () { "linux,code", 0xf },
					/* EV_SW type */
					Package () { "linux,input-type", 0x5 },
					Package () { "label", "pen_eject" },
					Package () { "gpios",
						Package () {
							^^PENH, 0, 0, 1 /* inserted active low */
						}
					},
				}
			})
		}
	}
}

Scope (\_SB.PCI0.I2C2)
{
	Name (FMCN, Package () { 87, 197, 26 })

	Device (DIGI)
	{
		Name (_HID, "ACPI0C50")
		Name (_CID, "PNP0C50")
		Name (_UID, 1)
		Name (_S0W, 4)
		Name (_PRW, Package () { BOARD_DIG_EJECT, 3 })

		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (
				BOARD_DIG_I2C_ADDR,
				ControllerInitiated,
				400000,
				AddressingMode7Bit,
				"\\_SB.PCI0.I2C2",
			)
			Interrupt (ResourceConsumer, Level, ActiveLow)
			{
				BOARD_DIG_IRQ
			}
		})

		/*
		 * Function 1 returns the offset in the I2C device register
		 * address space at which the HID descriptor can be read.
		 *
		 * Arg0 = UUID
		 * Arg1 = revision number of requested function
		 * Arg2 = requested function number
		 * Arg3 = function specific parameter
		 */
		Method (_DSM, 4, NotSerialized)
		{
			If (LEqual (Arg0, ToUUID
			            ("3cdff6f7-4267-4555-ad05-b30a3d8938de"))) {
				If (LEqual (Arg2, Zero)) {
					/* Function 0 - Query */
					If (LEqual (Arg1, One)) {
						/* Revision 1 Function 1 */
						Return (Buffer (One) { 0x03 })
					} Else {
						/* Revision 2+ not supported */
						Return (Buffer (One) { 0x00 })
					}
				} ElseIf (LEqual (Arg2, One)) {
					/* Function 1 - HID Descriptor Addr */
					Return (0x0001)
				} Else {
					/* Function 2+ not supported */
					Return (Buffer (One) { 0x00 })
				}
			} Else {
				Return (Buffer (One) { 0x00 })
			}
		}
	}
}
