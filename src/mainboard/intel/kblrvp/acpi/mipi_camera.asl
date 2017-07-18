/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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

Scope (\_SB.PCI0.I2C2)
{
	Device (PMIC)
	{
		Name (_HID, "INT3472") /* _HID: Hardware ID */
		Name (_UID, Zero)  // _UID: Unique ID */
		Name (_DDN, "TPS68470 PMIC")  /* _DDN: DOS Device Name */
		Name (CAMD, 0x64)

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x004D, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C2",
				0x00, ResourceConsumer, ,
			)
		})
	}

	Device (CAM0)
	{
		Name (_HID, "OVTID858")  /* _HID: Hardware ID */
		Name (_UID, Zero)  /* _UID: Unique ID */
		Name (_DDN, "OV 13858 Camera") /* _DDN: DOS Device Name */
		Name (CAMD, 0x02)

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_DEP, Package() {\_SB.PCI0.I2C2.PMIC})
		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x0010, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C2",
				0x00, ResourceConsumer, ,
				)
		})

		Method (SSDB, 0, Serialized)
		{
			Return (Buffer (0x5E)
			{
				/* 0000 */   0x00, 0x50, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
				/* 0008 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0010 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0018 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
				/* 0020 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0028 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0030 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0038 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0040 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0048 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x03,
				/* 0050 */   0x09, 0x00, 0x02, 0x01, 0x00, 0x01, 0x00, 0x36,
				/* 0058 */   0x6E, 0x01, 0x00, 0x00, 0x00, 0x00
			})
		}
	}

	Device (VCM0)
	{
		Name (_HID, "DWDWD000")  /* _HID: Hardware ID */
		Name (_UID, Zero)  /* _UID: Unique ID */
		Name (_DDN, "Dongwoon AF DAC") /* _DDN: DOS Device Name */
		Name (CAMD, 0x03)

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_DEP, Package() {\_SB.PCI0.I2C2.PMIC})
		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x000C, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C2",
				0x00, ResourceConsumer, ,
				)
		})
	}
}

Scope (\_SB.PCI0.I2C3)
{
	Device (PMIC)
	{
		Name (_HID, "INT3473") /* _HID: Hardware ID */
		Name (_UID, Zero)  /* _UID: Unique ID */
		Name (_DDN, "TPS68470 PMIC 2") /* _DDN: DOS Device Name */
		Name (CAMD, 0x64)

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x0049, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C3",
				0x00, ResourceConsumer, ,
			)
		})
	}

	Device (CAM1)
	{
		Name (_HID, "INT3479") /* _HID: Hardware ID */
		Name (_UID, Zero)  /* _UID: Unique ID */
		Name (_DDN, "OV 5670 Camera")  /* _DDN: DOS Device Name */
		Name (CAMD, 0x02)

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_DEP, Package() {\_SB.PCI0.I2C3.PMIC})
		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x0010, ControllerInitiated, 0x00061A80,
			AddressingMode7Bit, "\\_SB.PCI0.I2C3",
			0x00, ResourceConsumer, ,
			)
		})

		Method (SSDB, 0, Serialized)
		{
			Return (Buffer (0x5E)
			{
				/* 0000 */   0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0008 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0010 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0018 */   0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00,
				/* 0020 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0028 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0030 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0038 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0040 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0048 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x03,
				/* 0050 */   0x09, 0x00, 0x02, 0x01, 0x00, 0x01, 0x00, 0x36,
				/* 0058 */   0x6E, 0x01, 0x00, 0x00, 0x00, 0x00
			})
		}
	}
}
