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
Name(LOMH, 0x0)	/* Start of unused memory in C0000-E0000 range */
Name(PBAD, 0x0)	/* Address of BIOS area (If TOM2 != 0, Addr >> 16) */
Name(PBLN, 0x0)	/* Length of BIOS area */

Name(PCBA, CONFIG_MMCONF_BASE_ADDRESS)	/* Base address of PCIe config space */
Name(PCLN, Multiply(0x100000, CONFIG_MMCONF_BUS_NUMBER)) /* Length of PCIe config space, 1MB each bus */
Name(HPBA, 0xFED00000)	/* Base address of HPET table */

Name(SSFG, 0x0D)		/* S1 support: bit 0, S2 Support: bit 1, etc. S0 & S5 assumed */

/* Some global data */
Name(OSVR, 3)	/* Assume nothing. WinXp = 1, Vista = 2, Linux = 3, WinCE = 4 */
Name(OSV, Ones)	/* Assume nothing */
Name(PMOD, One)	/* Assume APIC */

/* AcpiGpe0Blk */
OperationRegion(GP0B, SystemMemory, 0xfed80814, 0x04)
	Field(GP0B, ByteAcc, NoLock, Preserve) {
	, 11,
	USBS, 1,
}
