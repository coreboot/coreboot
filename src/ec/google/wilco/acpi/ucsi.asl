/* SPDX-License-Identifier: GPL-2.0-only */

Device (UCSI)
{
	Name (_HID, "GOOG000E")
	Name (_CID, EisaId ("PNP0CA0"))
	Name (_DDN, "Wilco EC UCSI")
	Name (_UID, 1)
	Name (_STA, 0xb)

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

	Method (_DSM, 4, Serialized)
	{
		If (Arg0 != ToUUID ("6f8398c2-7ca4-11e4-ad36-631042b5008f")) {
			Return (Buffer (1) { Zero })
		}

		Switch (ToInteger (Arg2))
		{
			Case (Zero)
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
		}
		Return (Buffer (1) { Zero })
	}
}
