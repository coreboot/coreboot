/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
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

/* Memory related values */
Name (LOMH, 0x0)	/* Start of unused memory in C0000-E0000 range */
Name (PBAD, 0x0)	/* Address of BIOS area (If TOM2 != 0, Addr >> 16) */
Name (PBLN, 0x0)	/* Length of BIOS area */

/* Base address of PCIe config space */
Name (PCBA, CONFIG_MMCONF_BASE_ADDRESS)

/* Length of PCIe config space, 1MB each bus */
Name (PCLN, Multiply(0x100000, CONFIG_MMCONF_BUS_NUMBER))

/* Base address of HPET table */
Name (HPBA, 0xFED00000)

/* S1 support: bit 0, S2 Support: bit 1, etc. S0 & S5 assumed */
Name (SSFG, 0x0D)

/* Global Data */
Name (OSVR, 3)		/* WinXp = 1, Vista = 2, Linux = 3, WinCE = 4 */
Name (OSV, Ones)	/* Assume nothing */
Name (PMOD, One)	/* Assume APIC */

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
			"\\_SB.I2CD",		/* ResourceSource */
		)
		GpioInt (Level, ActiveLow, ExclusiveAndWake, PullNone,,
			"\\_SB.GPIO") { 0x5 }
	})

	/* Allow device to power off in S0 */
	Name (_S0W, 3)
}
