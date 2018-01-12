/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Google Inc.
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

Device (ETPA)
{
	Name (_HID, "ELAN0000")
	Name (_DDN, "Elan Touchpad")
	Name (_UID, 1)
	Name (ISTP, 1)	/* Touchpad */

	Name (_CRS, ResourceTemplate()
	{
		I2cSerialBus (
			0x15,			/* SlaveAddress */
			ControllerInitiated,	/* SlaveMode */
			400000,			/* ConnectionSpeed */
			AddressingMode7Bit,	/* AddressingMode */
			"\\_SB.I2CC",		/* ResourceSource */
		)
		GpioInt (Level, ActiveLow, ExclusiveAndWake, PullNone,,
			"\\_SB.GPIO") { 0x5 }
	})

	/* Allow device to power off in S0 */
	Name (_S0W, 3)
}
