/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Tablet-mode switch
 *
 * The EC determines whether we are in tablet mode and signals it on
 * EC_PAD_MODE (GPP_F11, open-drain: high = tablet, low = clamshell).
 * Here we use ACPI to tell the OS.
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
			/*
			 * GPP_F11: GPIO community 1 (GPP_F_E, \_SB.PCI0.GPI1),
			 * GPP_F is the first group, so ACPI pin = 11.
			 */
			GpioInt (Edge, ActiveBoth, ExclusiveAndWake, PullUp, 0x0000,
				"\\_SB.PCI0.GPI1", 0x00, ResourceConsumer, ,)
			{
				11
			}
		})
		Method (_STA, 0, NotSerialized)
		{
			Return (0x0F)
		}
	}
}
