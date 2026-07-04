/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Smart battery, read from EC RAM. The EC reports capacities in 10 mWh
 * units and voltage in mV, so the battery is described in power units
 * (mW/mWh): _BIF PowerUnit = 0 and capacity fields are multiplied by 10.
 * Present rate is reported by the EC as a signed current (mA) and is
 * converted to mW using the present voltage.
 */

Device (BAT0)
{
	Name (_HID, EisaId ("PNP0C0A"))
	Name (_UID, Zero)

	Method (_STA, 0, NotSerialized)
	{
		/* Battery present and functional. */
		Return (0x1F)
	}

	Method (_BIF, 0, NotSerialized)
	{
		Name (BPKG, Package (13)
		{
			Zero,		/* 0: power unit = mW/mWh */
			0xFFFFFFFF,	/* 1: design capacity */
			0xFFFFFFFF,	/* 2: last full charge capacity */
			One,		/* 3: rechargeable */
			0xFFFFFFFF,	/* 4: design voltage (mV) */
			Zero,		/* 5: warning capacity */
			Zero,		/* 6: low capacity */
			One,		/* 7: capacity granularity 1 */
			One,		/* 8: capacity granularity 2 */
			"",		/* 9: model number */
			"",		/* 10: serial number */
			"LION",		/* 11: battery type */
			"",		/* 12: OEM information */
		})

		If (!ECOK) {
			Return (BPKG)
		}

		BPKG [1] = (BDCC * 10)
		BPKG [2] = (BFCC * 10)
		BPKG [4] = BDVG
		BPKG [5] = ((BFCC * 10) / 10)	/* warn at ~10% */
		BPKG [6] = ((BFCC * 10) / 20)	/* low at ~5% */
		BPKG [9] = ToString (BMOD, 16)
		BPKG [10] = ToString (BSER, 16)
		BPKG [12] = ToString (BMFG, 8)
		Return (BPKG)
	}

	Method (_BST, 0, NotSerialized)
	{
		Name (PKG, Package (4) { Zero, Zero, Zero, Zero })

		If (!ECOK) {
			Return (PKG)
		}

		/* Present rate: EC value is a signed current in mA. */
		Local0 = BPRT
		If ((Local0 & 0x8000)) {
			/* Negative -> discharging. */
			PKG [0] = One
			Local0 = (0x00010000 - Local0)
		} ElseIf (Local0) {
			PKG [0] = 0x02		/* charging */
		}

		/* Convert mA to mW using present voltage. */
		PKG [1] = ((Local0 * BVTG) / 1000)
		PKG [2] = (BRMC * 10)		/* remaining capacity (mWh) */
		PKG [3] = BVTG			/* present voltage (mV) */
		Return (PKG)
	}
}
