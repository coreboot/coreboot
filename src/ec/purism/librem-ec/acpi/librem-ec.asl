/* SPDX-License-Identifier: GPL-2.0-only */

// Notifications:
//   0x80 - hardware backlight toggle
//   0x81 - backlight toggle
//   0x82 - backlight down
//   0x83 - backlight up
//   0x84 - backlight color change
Device (LIEC) {
	Name (_HID, "PURI4543")
	Name (_UID, 0)

	Method (RSET, 0, Serialized) {
		Printf ("LIEC: RSET")
		SAPL(0)
		SKBL(0)
	}

	Method (INIT, 0, Serialized) {
		Printf ("LIEC: INIT")
		RSET()
		If (^^PCI0.LPCB.EC0.ECOK) {
			// Set flags to use software control
			^^PCI0.LPCB.EC0.ECOS = 2
			Return (0)
		} Else {
			Return (1)
		}
	}

	Method (FINI, 0, Serialized) {
		Printf ("LIEC: FINI")
		RSET()
		If (^^PCI0.LPCB.EC0.ECOK) {
			// Set flags to use hardware control
			^^PCI0.LPCB.EC0.ECOS = 1
			Return (0)
		} Else {
			Return (1)
		}
	}

	// Get Airplane LED
	Method (GAPL, 0, Serialized) {
		If (^^PCI0.LPCB.EC0.ECOK) {
			If (^^PCI0.LPCB.EC0.WLED & 0x40) {
				Return (1)
			}
		}
		Return (0)
	}

	// Set Airplane LED
	Method (SAPL, 1, Serialized) {
		If (^^PCI0.LPCB.EC0.ECOK) {
			If (Arg0) {
				^^PCI0.LPCB.EC0.WLED |= 0x40
			} Else {
				^^PCI0.LPCB.EC0.WLED &= 0xBF
			}
		}
	}

	// Get notification red LED
	Method (GNTR, 0, Serialized) {
		If (^^PCI0.LPCB.EC0.ECOK) {
			Return (^^PCI0.LPCB.EC0.NOTR)
		}
		Return (0)
	}

	// Set notification red LED
	Method (SNTR, 1, Serialized) {
		If (^^PCI0.LPCB.EC0.ECOK) {
			^^PCI0.LPCB.EC0.NOTR = Arg0
		}
	}

	// Get notification green LED
	Method (GNTG, 0, Serialized) {
		If (^^PCI0.LPCB.EC0.ECOK) {
			Return (^^PCI0.LPCB.EC0.NOTG)
		}
		Return (0)
	}

	// Set notification green LED
	Method (SNTG, 1, Serialized) {
		If (^^PCI0.LPCB.EC0.ECOK) {
			^^PCI0.LPCB.EC0.NOTG = Arg0
		}
	}

	// Get notification blue LED
	Method (GNTB, 0, Serialized) {
		If (^^PCI0.LPCB.EC0.ECOK) {
			Return (^^PCI0.LPCB.EC0.NOTB)
		}
		Return (0)
	}

	// Set notification blue LED
	Method (SNTB, 1, Serialized) {
		If (^^PCI0.LPCB.EC0.ECOK) {
			^^PCI0.LPCB.EC0.NOTB = Arg0
		}
	}

	// Get KB LED
	Method (GKBL, 0, Serialized) {
		Local0 = 0
		If (^^PCI0.LPCB.EC0.ECOK) {
			^^PCI0.LPCB.EC0.FDAT = 1
			^^PCI0.LPCB.EC0.FCMD = 0xCA
			Local0 = ^^PCI0.LPCB.EC0.FBUF
			^^PCI0.LPCB.EC0.FCMD = Zero
		}
		Return (Local0)
	}

	// Set KB Led
	Method (SKBL, 1, Serialized) {
		If (^^PCI0.LPCB.EC0.ECOK) {
			^^PCI0.LPCB.EC0.FDAT = Zero
			^^PCI0.LPCB.EC0.FBUF = Arg0
			^^PCI0.LPCB.EC0.FCMD = 0xCA
		}
	}

	// Fan names
	Method (NFAN, 0, Serialized) {
		Return (Package() {
			"CPU fan",
		})
	}

	// Get fan duty cycle and RPM as a single value
	Method (GFAN, 1, Serialized) {
		Local0 = 0
		Local1 = 0
		If (^^PCI0.LPCB.EC0.ECOK) {
			If (Arg0 == 0) {
				Local0 = ^^PCI0.LPCB.EC0.DUT1
				Local1 = ^^PCI0.LPCB.EC0.RPM1
			} ElseIf (Arg0 == 1) {
				Local0 = ^^PCI0.LPCB.EC0.DUT2
				Local1 = ^^PCI0.LPCB.EC0.RPM2
			}
		}
		If (Local1 != 0) {
			// 60 * (EC frequency / 120) / 2
			Local1 = 2156250 / Local1
		}
		Return ((Local1 << 8) | Local0)
	}

	// Temperature names
	Method (NTMP, 0, Serialized) {
		Return (Package() {
			"CPU temp",
		})
	}

	// Get temperature
	Method (GTMP, 1, Serialized) {
		Local0 = 0;
		If (^^PCI0.LPCB.EC0.ECOK) {
			If (Arg0 == 0) {
				Local0 = ^^PCI0.LPCB.EC0.TMP1
			} ElseIf (Arg0 == 1) {
				Local0 = ^^PCI0.LPCB.EC0.TMP2
			}
		}
		Return (Local0)
	}
}
