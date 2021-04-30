/* SPDX-License-Identifier: GPL-2.0-only */

Device (HIDD)
{
	Name (_HID, "INT33D5")
	Name (HBSY, Zero)
	Name (HIDX, Zero)
	Name (HMDE, Zero)
	Name (HRDY, Zero)
	Name (BTLD, Zero)
	Name (BTS1, Zero)
	Name (HEB1, 0x3003)

	Method (_STA, 0, Serialized)  // _STA: Status
	{
		If ((OSYS >= 0x07DD))
		{
			Return (0x0F)
		}
		Else
		{
			Return (Zero)
		}
	}

	Method (HDDM, 0, Serialized)
	{
		Store ("-----> HDDM", Debug)
		Name (DPKG, Package (0x04)
		{
			0x11111111,
			0x22222222,
			0x33333333,
			0x44444444
		})
		Return (DPKG)
	}

	Method (HDEM, 0, Serialized)
	{
		Store ("-----> HDEM", Debug)
		HBSY = Zero
		If ((HMDE == Zero))
		{
			Return (HIDX)
		}
		Return (HMDE)
	}

	Method (HDMM, 0, Serialized)
	{
		Store ("-----> HDMM", Debug)
		Return (HMDE)
	}

	Method (HDSM, 1, Serialized)
	{
		Store ("-----> HDSM", Debug)
		HRDY = Arg0
	}

	Method (HPEM, 1, Serialized)
	{
		Store ("-----> HPEM", Debug)
		HBSY = One
		HIDX = Arg0

		Notify (HIDD, 0xC0)
		Local0 = Zero
		While ((Local0 < 0xFA) && HBSY)
		{
			Sleep (0x04)
			Local0++
		}

		If (HBSY == One)
		{
			HBSY = Zero
			HIDX = Zero
			Return (One)
		}
		Else
		{
			Return (Zero)
		}
	}

	Method (BTNL, 0, Serialized)
	{
		Store ("-----> BTNL", Debug)
		If (CondRefOf (\_SB.PWRB.PBST))
		{
			\_SB.PWRB.PBST = Zero
			Notify (PWRB, One) // Device Check
		}

		BTLD = One
//		If ((AEAB == One))
//		{
			BTS1 = 0x1F
			\_SB.PCI0.LPCB.H_EC.ECWT (BTS1, RefOf (\_SB.PCI0.LPCB.H_EC.BTEN))
//		}
//		Else
//		{
//			BTS1 = Zero
//		}
	}

	Method (BTNE, 1, Serialized)
	{
		Store ("-----> BTNE", Debug)
//		If ((AEAB == One))
//		{
			BTS1 = ((Arg0 & 0x1E) | One)
			\_SB.PCI0.LPCB.H_EC.ECWT (BTS1, RefOf (\_SB.PCI0.LPCB.H_EC.BTEN))
//		}
	}

	Method (BTNS, 0, Serialized)
	{
		Store ("-----> BTNS", Debug)
//		If ((AEAB == One))
//		{
			BTS1 = \_SB.PCI0.LPCB.H_EC.ECRD (RefOf (\_SB.PCI0.LPCB.H_EC.BTEN))
//		}
		Return (BTS1)
	}

	Method (BTNC, 0, Serialized)
	{
		Store ("-----> BTNC", Debug)
//		If ((AEAB == One))
//		{
			Return (0x1F)
//		}
//		Else
//		{
//			Return (Zero)
//		}
	}

	Name (HEB2, Zero)
	Method (HEBC, 0, Serialized)
	{
		Store ("-----> HEBC", Debug)
//		If ((AHDB == One))
//		{
//			Return (\HEB1)
//		}
//		Else
//		{
			Return (Zero)
//		}
	}

	Method (H2BC, 0, Serialized)
	{
		Store ("-----> H2BC", Debug)
//		If ((AHDB == One))
//		{
//			Return (\HEB1)
//		}
//		Else
//		{
			Return (Zero)
//		}
	}

	Method (HEEC, 0, Serialized)
	{
		Store ("-----> HEEC", Debug)
//		If ((AHDB == One))
//		{
			Return (HEB2) /* \_SB_.HIDD.HEB2 */
//		}
//		Else
//		{
//			Return (Zero)
//		}
	}

	Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
	{
		If ((Arg0 == ToUUID ("eeec56b3-4442-408f-a792-4edd4d758054")))
		{
			If ((One == ToInteger (Arg1)))
			{
				Switch (ToInteger (Arg2))
				{
					Case (Zero)
					{
						Return (Buffer (0x02)
						{
							0xFF, 0x03
						})
					}
					Case (One)
					{
						BTNL ()
					}
					Case (0x02)
					{
						Return (HDMM ())
					}
					Case (0x03)
					{
						HDSM (DerefOf (Arg3 [Zero]))
					}
					Case (0x04)
					{
						Return (HDEM ())
					}
					Case (0x05)
					{
						Return (BTNS ())
					}
					Case (0x06)
					{
						BTNE (DerefOf (Arg3 [Zero]))
					}
					Case (0x07)
					{
						Return (HEBC ())
					}
					Case (0x08)
					{
					}
					Case (0x09)
					{
						Return (H2BC ())
					}
				}
			}
		}

		Return (Buffer (One)
		{
			0x00
		})
	}
}

Method (PWPR, 0, Serialized)
{
	 Notify (HIDD, 0xCE)
}

Method (PWRR, 0, Serialized)
{
	Notify (HIDD, 0xCF)
}
