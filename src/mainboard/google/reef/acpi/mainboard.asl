/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#include "acpi/superio.asl"
#include "../gpio.h"

Scope (\_SB)
{
	Device (LID0)
	{
		Name (_HID, EisaId ("PNP0C0D"))
		Method (_LID, 0)
		{
			Return (\_SB.PCI0.LPCB.EC0.LIDS)
		}
	}

	Device (PWRB)
	{
		Name (_HID, EisaId ("PNP0C0C"))
	}
}

Scope (\_SB.PCI0.LPCB)
{
	/* Chrome OS Embedded Controller */
	#include "ec.asl"
}

Scope (\_SB.PCI0.I2C0)
{
	/* Headphone Codec */
	Device (HPDA)
	{
		Name (_HID, "DLGS7219")
		Name (_DDN, "Dialog DA7219 Codec")
		Name (_UID, 1)
		Name (_S0W, 4)
		Name (_DSD, Package () {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
			Package () { "dlg,micbias-lvl", 2600 },
			Package () { "dlg,mic-amp-in-sel", "diff" },
			},
			ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
			Package () {
				Package () {"da7219_aad", "DAAD"},
			}
		})

		Name (DAAD, Package () {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "dlg,btn-cfg", 50 },
				Package () { "dlg,mic-det-thr", 500 },
				Package () { "dlg,jack-ins-deb", 20 },
				Package () { "dlg,jack-det-rate", "32ms_64ms" },
				Package () { "dlg,jack-rem-deb", 1 },
				Package () { "dlg,a-d-btn-thr", 0xa },
				Package () { "dlg,d-b-btn-thr", 0x16 },
				Package () { "dlg,b-c-btn-thr", 0x21 },
				Package () { "dlg,c-mic-btn-thr", 0x3E },
				Package () { "dlg,btn-avg", 4 },
				Package () { "dlg,adc-1bit-rpt", 1 },
			}
		})

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				BOARD_HP_MIC_CODEC_I2C_ADDR,
				ControllerInitiated,
				400000,
				AddressingMode7Bit,
				"\\_SB.PCI0.I2C0",
			)
			Interrupt (ResourceConsumer, Level, ActiveLow)
			{
				BOARD_HP_MIC_CODEC_IRQ
			}
		})

		Method (_STA)
		{
			Return (0xF)
		}
	}
}
