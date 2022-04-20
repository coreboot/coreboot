/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.LPCB)
{
	Device (AC)
	{
		Name (_HID, "ACPI0003")
		Name (_PCL, Package () { LPCB })
		Name (ACFG, One)

		Method (_PSR, 0, NotSerialized)
		{
			Return (ACFG)
		}
	}
}

Scope (\_SB)
{
	Device (LEDS)
	{
		Name (_HID, "PRP0001")
		Name (_DDN, "GPIO LEDs device")

		Name (_CRS, ResourceTemplate () {
			GpioIo (
				Exclusive,			// Not shared
				PullNone,			// No need for pulls
				0,				// Debounce timeout
				0,				// Drive strength
				IoRestrictionOutputOnly,	// Only used as output
				"\\_SB.PCI0.GPIO",		// GPIO controller
				0)				// Must be 0
			{
				296,				// GPP_E8 - STATUSLED#
			}
		})

		Name (_DSD, Package () {
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "compatible", Package() { "gpio-leds" } },
			},
			ToUUID("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
			Package () {
				Package () {"led-0", "LED0"},
			}
		})

		/*
		 * For more information about these bindings see:
		 * Documentation/devicetree/bindings/leds/common.yaml,
		 * Documentation/devicetree/bindings/leds/leds-gpio.yaml and
		 * Documentation/firmware-guide/acpi/gpio-properties.rst.
		 */
		Name (LED0, Package () {
			ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () {"label", "blue:status"},
				Package () {"default-state", "keep"},
				Package () {"linux,default-trigger", "disk-activity"},
				Package () {"gpios", Package () {^LEDS, 0, 0, 1}},
				Package () {"retain-state-suspended", 1},
			}
		})
	}
}
