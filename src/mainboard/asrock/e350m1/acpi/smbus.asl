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

/* SMBUS Support */
Mutex (SBX0, 0x00)
OperationRegion (SMB0, SystemIO, 0xB00, 0x0C)
	Field (SMB0, ByteAcc, NoLock, Preserve) {
		HSTS,   8,  /* SMBUS status */
		SSTS,   8,  /* SMBUS slave status */
		HCNT,   8,  /* SMBUS control */
		HCMD,   8,  /* SMBUS host cmd */
		HADD,   8,  /* SMBUS address */
		DAT0,   8,  /* SMBUS data0 */
		DAT1,   8,  /* SMBUS data1 */
		BLKD,   8,  /* SMBUS block data */
		SCNT,   8,  /* SMBUS slave control */
		SCMD,   8,  /* SMBUS shadow cmd */
		SEVT,   8,  /* SMBUS slave event */
		SDAT,   8   /* SMBUS slave data */
}

Method (WCLR, 0, NotSerialized) { /* clear SMBUS status register */
	Store (0x1E, HSTS)
	Store (0xFA, Local0)
	While (LAnd (LNotEqual (And (HSTS, 0x1E), Zero), LGreater (Local0, Zero))) {
		Stall (0x64)
		Decrement (Local0)
	}

	Return (Local0)
}

Method (SWTC, 1, NotSerialized) {
	Store (Arg0, Local0)
	Store (0x07, Local2)
	Store (One, Local1)
	While (LEqual (Local1, One)) {
		Store (And (HSTS, 0x1E), Local3)
		If (LNotEqual (Local3, Zero)) { /* read sucess */
			If (LEqual (Local3, 0x02)) {
				Store (Zero, Local2)
			}

			Store (Zero, Local1)
		}
		Else {
			If (LLess (Local0, 0x0A)) { /* read failure */
				Store (0x10, Local2)
				Store (Zero, Local1)
			}
			Else {
				Sleep (0x0A) /* 10 ms, try again */
				Subtract (Local0, 0x0A, Local0)
			}
		}
	}

	Return (Local2)
}

Method (SMBR, 3, NotSerialized) {
	Store (0x07, Local0)
	If (LEqual (Acquire (SBX0, 0xFFFF), Zero)) {
		Store (WCLR (), Local0) /* clear SMBUS status register before read data */
		If (LEqual (Local0, Zero)) {
			Release (SBX0)
			Return (0x0)
		}

		Store (0x1F, HSTS)
		Store (Or (ShiftLeft (Arg1, One), One), HADD)
		Store (Arg2, HCMD)
		If (LEqual (Arg0, 0x07)) {
			Store (0x48, HCNT) /* read byte */
		}

		Store (SWTC (0x03E8), Local1) /* 1000 ms */
		If (LEqual (Local1, Zero)) {
			If (LEqual (Arg0, 0x07)) {
				Store (DAT0, Local0)
			}
		}
		Else {
			Store (Local1, Local0)
		}

		Release (SBX0)
	}

	/* DBGO("the value of SMBusData0 register ") */
	/* DBGO(Arg2) */
	/* DBGO(" is ") */
	/* DBGO(Local0) */
	/* DBGO("\n") */

	Return (Local0)
}

