/* SPDX-License-Identifier: GPL-2.0-only */

Device (\_SB.PCI0.PEG0)
{
	Name (_ADR, 0x00010000)

	Device (PEGP)
	{
		Name (_ADR, 0)

		// Convert a byte to a hex string, trimming extra parts
		Method (BHEX, 1)
		{
			Local0 = ToHexString(Arg0)
			Return (Mid(Local0, SizeOf(Local0) - 2, 2))
		}

		// UUID to string
		Method (IDST, 1)
		{
			Local0 = ""
			Fprintf(
				Local0,
				"%o%o%o%o-%o%o-%o%o-%o%o-%o%o%o%o%o%o",
				BHEX(DerefOf(Arg0[3])),
				BHEX(DerefOf(Arg0[2])),
				BHEX(DerefOf(Arg0[1])),
				BHEX(DerefOf(Arg0[0])),
				BHEX(DerefOf(Arg0[5])),
				BHEX(DerefOf(Arg0[4])),
				BHEX(DerefOf(Arg0[7])),
				BHEX(DerefOf(Arg0[6])),
				BHEX(DerefOf(Arg0[8])),
				BHEX(DerefOf(Arg0[9])),
				BHEX(DerefOf(Arg0[10])),
				BHEX(DerefOf(Arg0[11])),
				BHEX(DerefOf(Arg0[12])),
				BHEX(DerefOf(Arg0[13])),
				BHEX(DerefOf(Arg0[14])),
				BHEX(DerefOf(Arg0[15]))
			)
			Return (Local0)
		}

		// Safe hex conversion, checks type first
		Method (SFST, 1)
		{
			Local0 = ObjectType(Arg0)
			If (Local0 == 1 || Local0 == 2 || Local0 == 3) {
				Return (ToHexString(Arg0))
			} Else {
				Return (Concatenate("Type: ", Arg0))
			}
		}

		Method (_DSM, 4, Serialized)
		{
			Printf("NVIDIA _DSM")
			Printf("  Arg0: %o", IDST(Arg0))
			Printf("  Arg1: %o", SFST(Arg1))
			Printf("  Arg2: %o", SFST(Arg2))
			Printf("  Arg3: %o", SFST(Arg3))

			If (Arg0 == ToUUID ("d4a50b75-65c7-46f7-bfb7-41514cea0244")) {
				If (Arg1 != 0x0102) {
					Printf("  Invalid Arg1, return 0x80000002")
					Return (0x80000002)
				}

				If (Arg2 == 0) {
					Printf("  Arg2 == 0x00, return supported functions")
					Return (Buffer (4) { 0x01, 0x00, 0x10, 0x00 })
				}

				If (Arg2 == 0x14) {
					Printf("  Arg2 == 0x14, return backlight package")
					Return (Package (9) {
						0x8000A450,
						0x0200,
						0,
						0,
						1,
						1,
						200,
						32,
						1000
					})
				}

				Printf("  Unknown Arg2, return 0x80000002")
				Return (0x80000002)
			}

			Printf("  Unknown Arg0, return 0x80000001")
			Return (0x80000001)
		}

		// _DOD: Display Output Devices
		Method (_DOD, 0, NotSerialized)
		{
			Return (Package (3) {
				0x80008320,
				0x80006330,
				0x8000A450
			})
		}

		Device (HDM0)
		{
			Name (_ADR, 0x80008320)
		}

		Device (DSP0)
		{
			Name (_ADR, 0x80006330)
		}

		Device (DSP1)
		{
			Name (_ADR, 0x8000A450)
		}
	}
}
