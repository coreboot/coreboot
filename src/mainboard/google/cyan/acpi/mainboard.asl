/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <onboard.h>

Scope (\_SB)
{
	Device (LID0)
	{
		Name (_HID, EisaId ("PNP0C0D"))
		Method (_LID, 0)
		{
			Store (\_SB.PCI0.LPCB.EC0.LIDS, \LIDS)
			Return (\LIDS)
		}
	}

	Device (PWRB)
	{
		Name (_HID, EisaId ("PNP0C0C"))
		Name (_UID, 1)
	}
}


/*
 * LPC Trusted Platform Module
 */
Scope (\_SB.PCI0.LPCB)
{
	#include <drivers/pc80/tpm/acpi/tpm.asl>
}

Scope (\_SB.I2C1)
{
	Device (ETSA)
	{
		Name (_HID, "ELAN0001")
		Name (_DDN, "Elan Touchscreen ")
		Name (_UID, 5)
		Name (ISTP, 0) /* TouchScreen */

		Method(_CRS, 0x0, NotSerialized)
		{
			Name(BUF0,ResourceTemplate ()
			{
				I2CSerialBus(
					0x10,                     /* SlaveAddress */
					ControllerInitiated,      /* SlaveMode */
					400000,                   /* ConnectionSpeed */
					AddressingMode7Bit,       /* AddressingMode */
					"\\_SB.I2C1",             /* ResourceSource */
				)
				GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone,,
					 "\\_SB.GPSW") { BOARD_TOUCH_GPIO_INDEX }

			} )
			Name(BUF1,ResourceTemplate ()
			{
				I2CSerialBus(
					0x10,                     /* SlaveAddress */
					ControllerInitiated,      /* SlaveMode */
					400000,                   /* ConnectionSpeed */
					AddressingMode7Bit,       /* AddressingMode */
					"\\_SB.I2C1",             /* ResourceSource */
				)
				GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone,,
					 "\\_SB.GPNC") { BOARD_EVT_TOUCH_GPIO_INDEX }

			} )
			If (LEqual (\BDID, BOARD_EVT)) {
				Return (BUF1)
			} Else {
				Return (BUF0)
			}
		}

		Method (_STA)
		{
			If (LEqual (\S1EN, 1)) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}

		Name (_PRW, Package() { BOARD_TOUCHSCREEN_WAKE_GPIO, 0x3 })

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}
}

Scope (\_SB.I2C2)
{
	/* Maxim Audio Codec */
	Device (MAXM)   /* Audio Codec driver I2C */
	{
		Name (_ADR, 0)
		Name (_HID, AUDIO_CODEC_HID)
		Name (_CID, AUDIO_CODEC_CID)
		Name (_DDN, AUDIO_CODEC_DDN)
		Name (_UID, 1)

		/* Add DT style bindings with _DSD */
		Name (_DSD, Package () {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				/* set maxim micbias to 2.8v */
				Package () { "maxim,micbias", 3 },
			}
		})

		Method(_CRS, 0x0, NotSerialized)
		{
			Name(SBUF,ResourceTemplate ()
			{
				I2CSerialBus(
					AUDIO_CODEC_I2C_ADDR,	/* SlaveAddress: bus address */
					ControllerInitiated,	/* SlaveMode: default to ControllerInitiated */
					400000,			/* ConnectionSpeed: in Hz */
					AddressingMode7Bit,	/* Addressing Mode: default to 7 bit */
					"\\_SB.I2C2",		/* ResourceSource: I2C bus controller name */
				)

				GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone,,
					"\\_SB.GPSE") { BOARD_JACK_MAXIM_GPIO_INDEX }
			} )
			Return (SBUF)
		}

		Method (_STA)
		{
			If (LEqual (\S2EN, 1)) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}
	}
	Device (TISW)   /* TI Switch driver I2C */
	{
		Name (_ADR, 0)
		Name (_HID, TI_SWITCH_HID)
		Name (_CID, TI_SWITCH_CID)
		Name (_DDN, TI_SWITCH_DDN)
		Name (_UID, 1)

		/* Add DT style bindings with _DSD */
		Name (_DSD, Package () {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				/* set ti micbias to 2.8v */
				Package () { "ti,micbias", 7 },
			}
		})

		Method(_CRS, 0x0, NotSerialized)
		{
			Name(SBUF,ResourceTemplate ()
			{
				I2CSerialBus(
					TI_SWITCH_I2C_ADDR,	/* SlaveAddress: bus address */
					ControllerInitiated,	/* SlaveMode: default to ControllerInitiated */
					400000,	/* ConnectionSpeed: in Hz */
					AddressingMode7Bit,	/* Addressing Mode: default to 7 bit */
					"\\_SB.I2C2",	/* ResourceSource: I2C bus controller name */
				)

				GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone,,
					"\\_SB.GPSW") { BOARD_JACK_TI_GPIO_INDEX }

			} )
			Return (SBUF)
		}

		Method (_STA)
		{
			Return (0xF)
		}
	}
}

Scope (\_SB.I2C6)
{
	Device (ETPA)
	{
		Name (_HID, "ELAN0000")
		Name (_DDN, "Elan Touchpad")
		Name (_UID, 3)
		Name (ISTP, 1) /* Touchpad */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x15,                     /* SlaveAddress */
				ControllerInitiated,      /* SlaveMode */
				400000,                   /* ConnectionSpeed */
				AddressingMode7Bit,       /* AddressingMode */
				"\\_SB.I2C6",             /* ResourceSource */
			)
			GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone,,
				 "\\_SB.GPNC") { BOARD_TRACKPAD_GPIO_INDEX }
		})

		Method (_STA)
		{
			If (LEqual (\S6EN, 1)) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}

		Name (_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x3 })

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}
}

Scope (\_SB.LPEA)
{
	Name (GBUF, ResourceTemplate ()
	{
		/* Jack Detect (index 0) */
		GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone,,
			"\\_SB.GPSE") { BOARD_JACK_MAXIM_GPIO_INDEX }
	})
}

Scope (\_SB.GPNC)
{
	Method (_AEI, 0, NotSerialized)  // _AEI: ACPI Event Interrupts
	{
		Name (RBUF, ResourceTemplate ()
		{
			GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone,,
				"\\_SB.GPNC") { BOARD_SCI_GPIO_INDEX }
		})
		Return (RBUF)
	}

	Method (_E0F, 0, NotSerialized)  // _Exx: Edge-Triggered GPE
	{
	}
}
