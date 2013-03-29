/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Data to be patched by the BIOS during POST */
/* FIXME the patching is not done yet! */
/* Memory related values */
Name(LOMH, 0x0)	/* Start of unused memory in C0000-E0000 range */
Name(PBAD, 0x0)	/* Address of BIOS area (If TOM2 != 0, Addr >> 16) */
Name(PBLN, 0x0)	/* Length of BIOS area */

Name(PCBA, CONFIG_MMCONF_BASE_ADDRESS)	/* Base address of PCIe config space */
Name(HPBA, 0xFED00000)	/* Base address of HPET table */

Name(SSFG, 0x0D)	/* S1 support: bit 0, S2 Support: bit 1, etc. S0 & S5 assumed */

/* Some global data */
Name(OSTP, 3)	/* Assume nothing. WinXp = 1, Vista = 2, Linux = 3, WinCE = 4 */
Name(OSV, Ones)	/* Assume nothing */
Name(PMOD, One)	/* Assume APIC */

Scope(\_SB) {
	Method(CkOT, 0){

		if(LNotEqual(OSTP, Ones)) {Return(OSTP)}	/* OS version was already detected */

		if(CondRefOf(\_OSI,Local1))
		{
			Store(1, OSTP)					/* Assume some form of XP */
			if (\_OSI("Windows 2006"))		/* Vista */
			{
				Store(2, OSTP)
			}
		} else {
			If(WCMP(\_OS,"Linux")) {
				Store(3, OSTP)				/* Linux */
			} Else {
				Store(4, OSTP)				/* Gotta be WinCE */
			}
		}
		Return(OSTP)
	}
}

Scope(\_SI) {
	Method(_SST, 1) {
		/* DBGO("\\_SI\\_SST\n") */
		/* DBGO("   New Indicator state: ") */
		/* DBGO(Arg0) */
		/* DBGO("\n") */
	}
} /* End Scope SI */
