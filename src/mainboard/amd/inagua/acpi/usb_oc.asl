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
 */

/* simple name description */
/*
DefinitionBlock ("DSDT.AML","DSDT",0x01,"XXXXXX","XXXXXXXX",0x00010001
		)
	{
		#include "usb.asl"
	}
*/

/* USB overcurrent mapping pins.   */
Name(UOM0, 0)
Name(UOM1, 2)
Name(UOM2, 0)
Name(UOM3, 7)
Name(UOM4, 2)
Name(UOM5, 2)
Name(UOM6, 6)
Name(UOM7, 2)
Name(UOM8, 6)
Name(UOM9, 6)

Method(UCOC, 0) {
	Sleep(20)
	Store(0x13,CMTI)
	Store(0,GPSL)
}

/* USB Port 0 overcurrent uses Gpm 0 */
If(LLessEqual(UOM0,9)) {
	Scope (\_GPE) {
		Method (_L13) {
			UCOC()
			if(LEqual(GPB0,PLC0)) {
				Not(PLC0,PLC0)
				Store(PLC0, \_SB.PT0D)
			}
		}
	}
}

/* USB Port 1 overcurrent uses Gpm 1 */
If (LLessEqual(UOM1,9)) {
	Scope (\_GPE) {
		Method (_L14) {
			UCOC()
			if (LEqual(GPB1,PLC1)) {
				Not(PLC1,PLC1)
				Store(PLC1, \_SB.PT1D)
			}
		}
	}
}

/* USB Port 2 overcurrent uses Gpm 2 */
If (LLessEqual(UOM2,9)) {
	Scope (\_GPE) {
		Method (_L15) {
			UCOC()
			if (LEqual(GPB2,PLC2)) {
				Not(PLC2,PLC2)
				Store(PLC2, \_SB.PT2D)
			}
		}
	}
}

/* USB Port 3 overcurrent uses Gpm 3 */
If (LLessEqual(UOM3,9)) {
	Scope (\_GPE) {
		Method (_L16) {
			UCOC()
			if (LEqual(GPB3,PLC3)) {
				Not(PLC3,PLC3)
				Store(PLC3, \_SB.PT3D)
			}
		}
	}
}

/* USB Port 4 overcurrent uses Gpm 4 */
If (LLessEqual(UOM4,9)) {
	Scope (\_GPE) {
		Method (_L19) {
			UCOC()
			if (LEqual(GPB4,PLC4)) {
				Not(PLC4,PLC4)
				Store(PLC4, \_SB.PT4D)
			}
		}
	}
}

/* USB Port 5 overcurrent uses Gpm 5 */
If (LLessEqual(UOM5,9)) {
	Scope (\_GPE) {
		Method (_L1A) {
			UCOC()
			if (LEqual(GPB5,PLC5)) {
				Not(PLC5,PLC5)
				Store(PLC5, \_SB.PT5D)
			}
		}
	}
}

/* USB Port 6 overcurrent uses Gpm 6 */
If (LLessEqual(UOM6,9)) {
	Scope (\_GPE) {
		/* Method (_L1C) { */
		Method (_L06) {
			UCOC()
			if (LEqual(GPB6,PLC6)) {
				Not(PLC6,PLC6)
				Store(PLC6, \_SB.PT6D)
			}
		}
	}
}

/* USB Port 7 overcurrent uses Gpm 7 */
If (LLessEqual(UOM7,9)) {
	Scope (\_GPE) {
		/* Method (_L1D) { */
		Method (_L07) {
			UCOC()
			if (LEqual(GPB7,PLC7)) {
				Not(PLC7,PLC7)
				Store(PLC7, \_SB.PT7D)
			}
		}
	}
}

/* USB Port 8 overcurrent uses Gpm 8 */
If (LLessEqual(UOM8,9)) {
	Scope (\_GPE) {
		Method (_L17) {
			if (LEqual(G8IS,PLC8)) {
				Not(PLC8,PLC8)
				Store(PLC8, \_SB.PT8D)
			}
		}
	}
}

/* USB Port 9 overcurrent uses Gpm 9 */
If (LLessEqual(UOM9,9)) {
	Scope (\_GPE) {
		Method (_L0E) {
			if (LEqual(G9IS,0)) {
			Store(1,\_SB.PT9D)
			}
		}
	}
}
