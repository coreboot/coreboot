/* SPDX-License-Identifier: GPL-2.0-only */

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
	CMTI = 0x13
	GPSL = 0
}

/* USB Port 0 overcurrent uses Gpm 0 */
If (UOM0 <= 9) {
	Scope (\_GPE) {
		Method (_L13) {
			UCOC()
			if (GPB0 == PLC0) {
				PLC0 = ~PLC0
				\_SB.PT0D = PLC0
			}
		}
	}
}

/* USB Port 1 overcurrent uses Gpm 1 */
If (UOM1 <= 9) {
	Scope (\_GPE) {
		Method (_L14) {
			UCOC()
			if (GPB1 == PLC1) {
				PLC1 = ~PLC1
				\_SB.PT1D = PLC1
			}
		}
	}
}

/* USB Port 2 overcurrent uses Gpm 2 */
If (UOM2 <= 9) {
	Scope (\_GPE) {
		Method (_L15) {
			UCOC()
			if (GPB2 == PLC2) {
				PLC2 = ~PLC2
				\_SB.PT2D = PLC2
			}
		}
	}
}

/* USB Port 3 overcurrent uses Gpm 3 */
If (UOM3 <= 9) {
	Scope (\_GPE) {
		Method (_L16) {
			UCOC()
			if (GPB3 == PLC3) {
				PLC3 = ~PLC3
				\_SB.PT3D = PLC3
			}
		}
	}
}

/* USB Port 4 overcurrent uses Gpm 4 */
If (UOM4 <= 9) {
	Scope (\_GPE) {
		Method (_L19) {
			UCOC()
			if (GPB4 == PLC4) {
				PLC4 = ~PLC4
				\_SB.PT4D = PLC4
			}
		}
	}
}

/* USB Port 5 overcurrent uses Gpm 5 */
If (UOM5 <= 9) {
	Scope (\_GPE) {
		Method (_L1A) {
			UCOC()
			if (GPB5 == PLC5) {
				PLC5 = ~PLC5
				\_SB.PT5D = PLC5
			}
		}
	}
}

/* USB Port 6 overcurrent uses Gpm 6 */
If (UOM6 <= 9) {
	Scope (\_GPE) {
		/* Method (_L1C) { */
		Method (_L06) {
			UCOC()
			if (GPB6 == PLC6) {
				PLC6 = ~PLC6
				\_SB.PT6D = PLC6
			}
		}
	}
}

/* USB Port 7 overcurrent uses Gpm 7 */
If (UOM7 <= 9) {
	Scope (\_GPE) {
		/* Method (_L1D) { */
		Method (_L07) {
			UCOC()
			if (GPB7 == PLC7) {
				PLC7 = ~PLC7
				\_SB.PT7D = PLC7
			}
		}
	}
}

/* USB Port 8 overcurrent uses Gpm 8 */
If (UOM8 <= 9) {
	Scope (\_GPE) {
		Method (_L17) {
			if (G8IS == PLC8) {
				PLC8 = ~PLC8
				\_SB.PT8D = PLC8
			}
		}
	}
}

/* USB Port 9 overcurrent uses Gpm 9 */
If (UOM9 <= 9) {
	Scope (\_GPE) {
		Method (_L0E) {
			if (G9IS == 0) {
			\_SB.PT9D = 1
			}
		}
	}
}
