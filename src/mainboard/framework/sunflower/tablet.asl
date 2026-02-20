/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Tablet-mode switch

 * EC uses dual-input hall sensor and two GMR sensors (on mainboard and lid)
 * to determine if we're in tablet mode or not and here we use ACPI to tell the OS.
 */
Scope (\_SB)
{
	/* Equivalent to GOOG0006. Matched by Windows driver, Linux uses
	 * EC feature flags to match tablet/sensor behavior. No need for
	 * dynamic shmem read as in tbmc.asl */
	Device (TBMC)
	{
		Name (_HID, "FRMWC006")
		Name (_UID, 1)
		Name (_DDN, "Tablet Motion Control")

		Method(_STA, 0)
		{
			Return (0xF)
		}
	}

	Device (TBMD)
	{
		Name (_HID, "INT33D3")
		Name (_CID, "PNP0C60")  /* Display Sensor Device */
		Name (_DDN, "Tablet Mode Switch")
		Name (_CRS, ResourceTemplate ()
		{
			/* GPP_F12: GPP_F ACPI base 288 + 12 = 300 (0x12C) */
			GpioInt (Edge, ActiveBoth, ExclusiveAndWake, PullUp, 0x0000,
				"\\_SB.PCI0.GPIO", 0x00, ResourceConsumer, ,)
			{
				300
			}
		})
		Method (_STA, 0, NotSerialized)
		{
			Return (0x0F)
		}
	}
}
