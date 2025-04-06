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
		PWSL = PWST
	}

	/* EmbeddedControl Mutex */
	Mutex (ECLK, 0)

	OperationRegion(ERAM, EmbeddedControl, 0, 256)
	Field(ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0x00),
				ACPR, 1, /* AC Present */
				BPRT, 1, /* Battery Present */
				    , 2,
				LIDS, 1, /* Lid State */
		Offset (0x05),
				SCIE, 1, /* SCI Enable */
		Offset (0x06),
				/* Power status:
				   Bit 0: AC Present */
				PWST, 8,
		Offset (0x07),
				EVT1, 8, /* Event 1 */
				EVT2, 8, /* Event 1 */
		Offset (0x0b),
				EVT3, 8, /* Event 3 */
				EVT4, 8, /* Event 4 */
	}

	Name (PWSL, 0) /* Last power status */
	Method (_Q66)
	{
		Local0 = EVT1
		/* Lid event */
		If (Local0 & (1 << 2)) {
			Notify(LID, 0x80)
		}
		/* Power event */
		If (Local0 & (1 << 3)) {
			/* Cache current power state as reading it clears bit 4
			   (battery status changed flag) */
			Local1 = PWST

			/* Check which state flags changed from last state */
			Local2 = PWSL ^ Local1
			PWSL = Local1
			If (Local2 & (1 << 0)) {
				Notify(AC, 0x80)
			}
			/* Battery present status changed, call _BIF */
			If (Local2 & (1 << 1)) {
				Notify(BAT0, 0x81)
			}
			/* Battery status changed, call _BST */
			If (Local1 & (1 << 3)) {
				Notify(BAT0, 0x80)
			}
		}
		If (Local0 & (1 << 6)) {
			Notify(SLPB, 0x80)
		}
	}

	Device (SLPB)
	{
		Name (_HID, EisaId("PNP0C0E"))
	}
	#include "ac.asl"
	#include "battery.asl"
	#include "ec_mailbox.asl"
	#include "lid.asl"
}
