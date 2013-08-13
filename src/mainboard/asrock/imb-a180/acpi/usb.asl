/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* simple name description */
/*
DefinitionBlock ("DSDT.AML","DSDT",0x01,"XXXXXX","XXXXXXXX",0x00010001
		)
	{
		#include "usb.asl"
	}
*/
Method(UCOC, 0) {
	Sleep(20)
	Store(0x13,CMTI)
	Store(0,GPSL)
}

/* USB Port 0 overcurrent uses Gpm 0 */
If(LLessEqual(UOM0,9)) {
	Scope (\_GPE) {
		Method (_L13) {
		}
	}
}

/* USB Port 1 overcurrent uses Gpm 1 */
If (LLessEqual(UOM1,9)) {
	Scope (\_GPE) {
		Method (_L14) {
		}
	}
}

/* USB Port 2 overcurrent uses Gpm 2 */
If (LLessEqual(UOM2,9)) {
	Scope (\_GPE) {
		Method (_L15) {
		}
	}
}

/* USB Port 3 overcurrent uses Gpm 3 */
If (LLessEqual(UOM3,9)) {
	Scope (\_GPE) {
		Method (_L16) {
		}
	}
}

/* USB Port 4 overcurrent uses Gpm 4 */
If (LLessEqual(UOM4,9)) {
	Scope (\_GPE) {
		Method (_L19) {
		}
	}
}

/* USB Port 5 overcurrent uses Gpm 5 */
If (LLessEqual(UOM5,9)) {
	Scope (\_GPE) {
		Method (_L1A) {
		}
	}
}

/* USB Port 6 overcurrent uses Gpm 6 */
If (LLessEqual(UOM6,9)) {
	Scope (\_GPE) {
		/* Method (_L1C) { */
		Method (_L06) {
		}
	}
}

/* USB Port 7 overcurrent uses Gpm 7 */
If (LLessEqual(UOM7,9)) {
	Scope (\_GPE) {
		/* Method (_L1D) { */
		Method (_L07) {
		}
	}
}

/* USB Port 8 overcurrent uses Gpm 8 */
If (LLessEqual(UOM8,9)) {
	Scope (\_GPE) {
		Method (_L17) {
		}
	}
}

/* USB Port 9 overcurrent uses Gpm 9 */
If (LLessEqual(UOM9,9)) {
	Scope (\_GPE) {
		Method (_L0E) {
		}
	}
}
