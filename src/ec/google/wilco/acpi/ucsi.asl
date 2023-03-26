/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB)
{
	Device (UCSI)
	{
		Name (_HID, EisaId("USBC000"))
		Name (_CID, Package { "GOOG000E", EisaId ("PNP0CA0") })
		Name (_DDN, "Wilco EC UCSI")
		Name (_UID, 1)
		Name (_STA, 0xf)

		/* Value written to EC control register to start UCSI command */
		Name (UCMD, 0xE0)

		/* Shared memory fields are defined in the SSDT */
		External (VER0, FieldUnitObj)
		External (VER1, FieldUnitObj)
		External (CCI0, FieldUnitObj)
		External (CCI1, FieldUnitObj)
		External (CCI2, FieldUnitObj)
		External (CCI3, FieldUnitObj)
		External (CTL0, FieldUnitObj)
		External (CTL1, FieldUnitObj)
		External (CTL2, FieldUnitObj)
		External (CTL3, FieldUnitObj)
		External (CTL4, FieldUnitObj)
		External (CTL5, FieldUnitObj)
		External (CTL6, FieldUnitObj)
		External (CTL7, FieldUnitObj)
		External (MGI0, FieldUnitObj)
		External (MGI1, FieldUnitObj)
		External (MGI2, FieldUnitObj)
		External (MGI3, FieldUnitObj)
		External (MGI4, FieldUnitObj)
		External (MGI5, FieldUnitObj)
		External (MGI6, FieldUnitObj)
		External (MGI7, FieldUnitObj)
		External (MGI8, FieldUnitObj)
		External (MGI9, FieldUnitObj)
		External (MGIA, FieldUnitObj)
		External (MGIB, FieldUnitObj)
		External (MGIC, FieldUnitObj)
		External (MGID, FieldUnitObj)
		External (MGIE, FieldUnitObj)
		External (MGIF, FieldUnitObj)
		External (MGO0, FieldUnitObj)
		External (MGO1, FieldUnitObj)
		External (MGO2, FieldUnitObj)
		External (MGO3, FieldUnitObj)
		External (MGO4, FieldUnitObj)
		External (MGO5, FieldUnitObj)
		External (MGO6, FieldUnitObj)
		External (MGO7, FieldUnitObj)
		External (MGO8, FieldUnitObj)
		External (MGO9, FieldUnitObj)
		External (MGOA, FieldUnitObj)
		External (MGOB, FieldUnitObj)
		External (MGOC, FieldUnitObj)
		External (MGOD, FieldUnitObj)
		External (MGOE, FieldUnitObj)
		External (MGOF, FieldUnitObj)

		Method (INIT)
		{
			/* Read UCSI version from EC into shared memory */
			^VER0 = R (^^UVR0)
			^VER1 = R (^^UVR1)
		}

		Method (R, 1, Serialized, 2){
			Local0 = \_SB.PCI0.LPCB.EC0.R(Arg0)
			Return (Local0)
		}

		Method (W, 2, Serialized, 2){
			\_SB.PCI0.LPCB.EC0.W(Arg0, Arg1)
		}

		Method (_DSM, 4, Serialized)
		{
			If (Arg0 != ToUUID ("6f8398c2-7ca4-11e4-ad36-631042b5008f")) {
				Return (Buffer (1) { 0 })
			}

			Switch (ToInteger (Arg2))
			{
				Case (0)
				{
					Return (Buffer (1) { 0x07 })
				}
				Case (1)
				{
					/* Write Message Out */
					W (^^UMO0, ^MGO0)
					W (^^UMO1, ^MGO1)
					W (^^UMO2, ^MGO2)
					W (^^UMO3, ^MGO3)
					W (^^UMO4, ^MGO4)
					W (^^UMO5, ^MGO5)
					W (^^UMO6, ^MGO6)
					W (^^UMO7, ^MGO7)
					W (^^UMO8, ^MGO8)
					W (^^UMO9, ^MGO9)
					W (^^UMOA, ^MGOA)
					W (^^UMOB, ^MGOB)
					W (^^UMOC, ^MGOC)
					W (^^UMOD, ^MGOD)
					W (^^UMOE, ^MGOE)
					W (^^UMOF, ^MGOF)

					/* Write Control */
					W (^^UCL0, ^CTL0)
					W (^^UCL1, ^CTL1)
					W (^^UCL2, ^CTL2)
					W (^^UCL3, ^CTL3)
					W (^^UCL4, ^CTL4)
					W (^^UCL5, ^CTL5)
					W (^^UCL6, ^CTL6)
					W (^^UCL7, ^CTL7)

					/* Start EC Command */
					W (^^UCTL, ^UCMD)
				}
				Case (2)
				{
					/* Read Message In */
					RMSG()
				}
			}
			Return (Buffer (1) { 0 })
		}

		Method (RMSG, 0, Serialized){
			/* Read Message In */
			^MGI0 = R (^^UMI0)
			^MGI1 = R (^^UMI1)
			^MGI2 = R (^^UMI2)
			^MGI3 = R (^^UMI3)
			^MGI4 = R (^^UMI4)
			^MGI5 = R (^^UMI5)
			^MGI6 = R (^^UMI6)
			^MGI7 = R (^^UMI7)
			^MGI8 = R (^^UMI8)
			^MGI9 = R (^^UMI9)
			^MGIA = R (^^UMIA)
			^MGIB = R (^^UMIB)
			^MGIC = R (^^UMIC)
			^MGID = R (^^UMID)
			^MGIE = R (^^UMIE)
			^MGIF = R (^^UMIF)

			/* Read Status */
			^CCI0 = R (^^UCI0)
			^CCI1 = R (^^UCI1)
			^CCI2 = R (^^UCI2)
			^CCI3 = R (^^UCI3)
		}

		// GPLD: Generate Port Location Data (PLD)
		Method (GPLD, 2, Serialized)
		{
			Name (PCKG, Package (0x01)
			{
				Buffer (0x10){}
			})

			// REV: Revision 0x02 for ACPI 5.0
			CreateField (DerefOf (PCKG[0]), 0, 0x07, REV)
			REV = 0x02

			// VISI: Port visibility to user per port
			CreateField (DerefOf (PCKG[0]), 0x40, 1, VISI)
			VISI = Arg0

			CreateField (DerefOf (PCKG[0]), 0x57, 0x08, GPOS)
			GPOS = Arg1

			CreateField (DerefOf (PCKG[0]), 0x4A, 0x04, SHAP)
			SHAP = 0x01

			CreateField (DerefOf (PCKG[0]), 0x20, 0x10, WID)
			WID = 0x08

			CreateField (DerefOf (PCKG[0]), 0x30, 0x10, HGT)
			HGT = 0x03
			Return (PCKG)
		}

		Method (GUPC, 1, Serialized)
		{
			Name (PCKG, Package (0x04)
			{
				One,
				Zero,
				Zero,
				Zero
			})
			PCKG[1] = Arg0
			Return (PCKG)
		}

		Device (TC01)
		{
			Name (_ADR, 0)  // _ADR: Address
			Method (_UPC, 0, NotSerialized)  // _UPC: USB Port Capabilities
			{
				Return (GUPC (0x09))
			}

			Method (_PLD, 0, NotSerialized)  // _PLD: Physical Location of Device
			{
				Return (GPLD (1, 1))
			}
		}
#ifdef EC_BOARD_HAS_2ND_TYPEC_PORT
		Device (TC02)
		{
			Name (_ADR, 1)  // _ADR: Address
			Method (_UPC, 0, NotSerialized)  // _UPC: USB Port Capabilities
			{
				Return (GUPC (0x09))
			}

			Method (_PLD, 0, NotSerialized)  // _PLD: Physical Location of Device
			{
				Return (GPLD (1, 2))
			}
		}
#endif
	}
}
