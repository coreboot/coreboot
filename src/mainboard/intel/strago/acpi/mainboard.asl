/* SPDX-License-Identifier: GPL-2.0-only */

#include "onboard.h"

Scope (\_SB)
{
	Device (PWRB)
	{
		Name (_HID, EisaId ("PNP0C0C"))
		Name (_UID, 1)
	}
}

Scope (\_SB.PCI0.I2C1)
{
	Device (ATSB)
	{
		Name (_HID, "ATML0001")
		Name (_DDN, "Atmel Touchscreen Bootloader")
		Name (_UID, 4)
		Name (ISTP, 0) /* TouchScreen */

		Method(_CRS, 0x0, Serialized)
		{
			Name (BUF0, ResourceTemplate ()
			{
				I2cSerialBus(
					0x26,                     /* SlaveAddress */
					ControllerInitiated,      /* SlaveMode */
					400000,                   /* ConnectionSpeed */
					AddressingMode7Bit,       /* AddressingMode */
					"\\_SB.PCI0.I2C1",        /* ResourceSource */
				)
				Interrupt (ResourceConsumer, Edge, ActiveLow)
				{
					BOARD_TOUCH_IRQ
				}
			})
			Return (BUF0)
		}

		Method (_STA)
		{
			If (\S1EN == 1) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}

	Device (ATSA)
	{
		Name (_HID, "ATML0001")
		Name (_DDN, "Atmel Touchscreen")
		Name (_UID, 5)
		Name (ISTP, 0) /* TouchScreen */

		Method(_CRS, 0x0, Serialized)
		{
			Name (BUF0, ResourceTemplate ()
			{
				I2cSerialBus(
					0x4b,                     /* SlaveAddress */
					ControllerInitiated,      /* SlaveMode */
					400000,                   /* ConnectionSpeed */
					AddressingMode7Bit,       /* AddressingMode */
					"\\_SB.PCI0.I2C1",        /* ResourceSource */
				)
				Interrupt (ResourceConsumer, Edge, ActiveLow)
				{
					BOARD_TOUCH_IRQ
				}
			})
			Return (BUF0)
		}

		Method (_STA)
		{
			If (\S1EN == 1) {
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

Scope (\_SB.PCI0.I2C5)
{
	/* Realtek Audio Codec */
	Device (RTEK)   /* Audio Codec driver I2C */
	{

		Name (_HID, AUDIO_CODEC_HID)
		Name (_CID, AUDIO_CODEC_CID)
		Name (_DDN, AUDIO_CODEC_DDN)
		Name (_UID, 1)

		Method(_CRS, 0x0, Serialized)
		{
			Name(SBUF,ResourceTemplate ()
			{
				I2CSerialBus(
					AUDIO_CODEC_I2C_ADDR,	/* SlaveAddress: bus address */
					ControllerInitiated,	/* SlaveMode: default to ControllerInitiated */
					400000,			/* ConnectionSpeed: in Hz */
					AddressingMode7Bit,	/* Addressing Mode: default to 7 bit */
					"\\_SB.PCI0.I2C5"	/* ResourceSource: I2C bus controller name */
				)

			 /* Jack Detect (index 0) */
			 GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone,,
				  "\\_SB.GPSW") { JACK_DETECT_GPIO_INDEX }
			} )
			Return (SBUF)
		}

		Method (_STA)
		{
			Return (0xF)
		}
	}
}

Scope (\_SB.PCI0.I2C6)
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
				"\\_SB.PCI0.I2C6",        /* ResourceSource */
			)
			GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone,,
				 "\\_SB.GPNC") { BOARD_TRACKPAD_GPIO_INDEX }
		})

		Method (_STA)
		{
			If (\S6EN == 1) {
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

Scope (\_SB.PCI0.LPEA)
{
	Name (GBUF, ResourceTemplate ()
	{
		/* Jack Detect (index 0) */
		GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone,,
			 "\\_SB.GPSW") { JACK_DETECT_GPIO_INDEX }
	})
}

Scope (\_SB.GPNC)
{
	Method (_AEI, 0, Serialized)  // _AEI: ACPI Event Interrupts
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
