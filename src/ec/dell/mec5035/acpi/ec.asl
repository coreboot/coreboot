/* SPDX-License-Identifier: GPL-2.0-only */

Device (EC)
{
	Name (_HID, EisaId("PNP0C09"))

	Name (_CRS, ResourceTemplate()
	{
		IO (Decode16, 0x930, 0x930, 0, 1)
		IO (Decode16, 0x934, 0x934, 0, 1)
	})

	/* Mainboard must define MEC5035_EC_GPE with the GPE number
	 * corresponding to the GPIO the EC SCI is routed to */
	Name (_GPE, MEC5035_EC_GPE)

	Method (_REG, 2, NotSerialized)
	{
		/* Enable SCIs once the OS ACPI EC driver connects
		 * the EmbeddedControl operation region */
		If (Arg0 == 3) {
			SCIE = Arg1
		}
	}

	/* EmbeddedControl Mutex */
	Mutex (ECLK, 0)

	OperationRegion(ERAM, EmbeddedControl, 0, 256)
	Field(ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0x05),
				SCIE, 1, /* SCI Enable */
		Offset (0x07),
				EVT1, 8, /* Event 1 */
				EVT2, 8, /* Event 1 */
		Offset (0x0b),
				EVT3, 8, /* Event 3 */
				EVT4, 8, /* Event 4 */
	}

	Method (_Q66)
	{
	}
}
