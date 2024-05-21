/* SPDX-License-Identifier: GPL-2.0-only */

Device (DASH) {
	Name (_HID, "DSHR4543")
	Name (_UID, 0)
	// Hide the device so that Windows does not warn about a missing driver.
	Name (_STA, 0xB)

	Method (INIT, 0, Serialized) {
		Printf ("DASH: INIT")
		If (^^PCI0.LPCB.EC0.ECOK) {
			// Set flags to use software control
			^^PCI0.LPCB.EC0.ECOS = 2
			Return (0)
		} Else {
			Return (1)
		}
	}

	Method (FINI, 0, Serialized) {
		Printf ("DASH: FINI")
		If (^^PCI0.LPCB.EC0.ECOK) {
			// Set flags to use hardware control
			^^PCI0.LPCB.EC0.ECOS = 1
			Return (0)
		} Else {
			Return (1)
		}
	}

	// Fan names
	Method (NFAN, 0, Serialized) {
		Return (Package() {
			"CPU fan",
#if CONFIG(EC_DASHARO_EC_DGPU)
			"GPU fan",
#endif
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
#if CONFIG(EC_DASHARO_EC_DGPU)
			"GPU temp",
#endif
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
