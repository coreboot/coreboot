/* SPDX-License-Identifier: GPL-2.0-only */

Device (EC0)
{
	Name (_HID, EISAID ("PNP0C09"))
	Name (_UID, 0)

	Name (_GPE, THINKPAD_EC_GPE)
	Mutex (ECLK, 0x07)

	/* EmbeddedControl should work as well, but memory mapped is faster. */
	OperationRegion (ERAM, SystemMemory, (CONFIG_EC_BASE_ADDRESS + 0x100), 0x100)
	Field (ERAM, ByteAcc, Lock, Preserve)
	{
		    ,   1,
		    ,   1,
		HKFA,   1,  // FN lock (Hotkey / FN row toggle)
		    ,   1,
		    ,   1,
		    ,   1,
		    ,   1,

		Offset(0x0c),
		HLCL,   8,  // LED control

		Offset(0x46),
		    ,   4,
		ONAC,   1,
		    ,   3,

		Offset(0x81),
		PAGE,   8,
	}

	/* Battery info page 0 */
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset(0xA0),
		SBRC,   16,		/* Remaining capacity */
		SBFC,   16,		/* Full-charge capacity */
		SBAE,   16,
		SBRS,   16,
		SBAC,   16,		/* Present rate */
		SBVO,   16,		/* Present voltage */
		SBAF,   16,
		SBBS,   16,
	}

	/* Battery info page 1 */
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0xA0),
                    ,   15,
		SBCM,   1,		/* Power unit (0 == mA/mAh, 1 == mW/mWh) */
		SBMD,   16,
		SBCC,   16,
	}

	/* Battery info page 2 */
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0xA0),
		SBDC,   16,		/* Design Capacity */
		SBDV,   16,		/* Design Voltage */
		SBOM,   16,
		SBSI,   16,
		SBDT,   16,
		SBSN,   16,		/* Serial number */
	}

	/* Battery info page 4 */
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0xA0),
		SBCH,   32,		/* Type (LiON) */
	}

	/* Battery info page 5 */
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0xA0),
		SBMN,   128,		/* Manufacturer */
	}

	/* Battery info page 6 */
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0xA0),
		SBDN,   128,		/* Model */
	}

	Method (_CRS, 0, Serialized)
	{
		Name (ECMD, ResourceTemplate()
		{
			IO (Decode16, 0x62, 0x62, 0, 1)
			IO (Decode16, 0x66, 0x66, 0, 1)
			IO (Decode16, 0x700, 0x700, 0, 8)
			Memory32Fixed(ReadOnly, CONFIG_EC_BASE_ADDRESS, 0x1000, FF00)
		})
		Return (ECMD)
	}

	/* Increase brightness hotkey */
	Method (_Q14, 0, NotSerialized) {
		BRIGHTNESS_UP()
		^HKEY.MHKQ (0x1010)
	}

	/* Decrease brightness hotkey */
	Method (_Q15, 0, NotSerialized) {
		BRIGHTNESS_DOWN()
		^HKEY.MHKQ (0x1011)
	}

	/* Video output switch hotkey */
	Method (_Q16, 0, NotSerialized) {
		Notify (\_SB.PCI0.GFX0, 0x82)
		^HKEY.MHKQ (0x1007)
	}

	Method (_Q19, 0, NotSerialized)
	{
		^HKEY.MHKQ (0x1018)
	}

	/* Switched to AC power */
	Method (_Q26, 0, NotSerialized)
	{
		PWRS = 1
		Notify (^AC, 0x80)
		Notify (^BAT0, 0x80)
		\PNOT ()
		^HKEY.MHKQ (0x6040)
	}

	/* Switched to battery power */
	Method (_Q27, 0, NotSerialized)
	{
		PWRS = 0
		Notify (^AC, 0x80)
		Notify (^BAT0, 0x80)
		\PNOT ()
		^HKEY.MHKQ (0x6040)
	}

	/* Lid opened */
	Method (_Q2A, 0, NotSerialized)
	{
		LIDS = 1
		Notify(^LID, 0x80)
		^HKEY.MHKQ (0x5002)
	}

	/* Lid closed */
	Method (_Q2B, 0, NotSerialized)
	{
		LIDS = 0
		Notify(^LID, 0x80)
		^HKEY.MHKQ (0x5001)
	}

	/* Tablet swivel down */
	Method (_Q35, 0, NotSerialized)
	{
		^HKEY.MHKQ (0x60c0)
		^HKEY.MHKQ (0x500a)
	}

	/* Tablet swivel up */
	Method (_Q36, 0, NotSerialized)
	{
		^HKEY.MHKQ (0x60c0)
		^HKEY.MHKQ (0x5009)
	}

	/* Tablet rotate lock button */
	Method (_Q37, 0, NotSerialized)
	{
		^HKEY.MHKQ (0x6020)
	}

	/* Numlock pressed */
	Method (_Q3F, 0, NotSerialized)
	{
		^HKEY.MHKQ (0x6000)
	}

	/* RFKill status changed */
	Method (_Q41, 0, NotSerialized) {
		^HKEY.MHKQ (0x7000)
	}

	/* Mute hotkey */
	Method (_Q43, 0, NotSerialized) {
		^HKEY.MHKQ (0x1017)
	}

	/* Settings hotkey */
	Method (_Q5A, 0, NotSerialized)
	{
		^HKEY.MHKQ (0x101D)
	}

	/* Search hotkey */
	Method (_Q5B, 0, NotSerialized)
	{
		^HKEY.MHKQ (0x101E)
	}

	/* Scale hotkey */
	Method (_Q5C, 0, NotSerialized)
	{
		^HKEY.MHKQ (0x101F)
	}

	/* File hotkey */
	Method (_Q5D, 0, NotSerialized)
	{
		^HKEY.MHKQ (0x1020)
	}

	/* RFkill hotkey */
	Method (_Q64, 0, NotSerialized) {
		^HKEY.MHKQ (0x1005)
	}

	/* Mic mute hotkey */
	Method (_Q6A, 0, NotSerialized)
	{
		^HKEY.MHKQ (0x101B)
	}

	/* FN key pressed */
	Method (_Q70, 0, NotSerialized) { }

	/* FN+Esc pressed / FN row mode switch */
	Method (_Q74, 0, NotSerialized) {
		HKFA = 1 ^ HKFA
		^HKEY.MHKQ (0x6060)
	}

	/* Implements enough of the Lenovo hotkey device for
	 * thinkpad-acpi to work, no real support for hotkey masking */
	Device (HKEY)
	{
		Name (_HID, EisaId ("LEN0068"))

		Name (DHKC, 0)
		Name (DHKV, 0)
		Name (DHKN, 0xFC018070)  // Hotkey mask
		Mutex (XDHK, 0x07)

		Method (_STA, 0, NotSerialized)
		{
			Return (0x0F)
		}

		Method (MHKV, 0, NotSerialized)
		{
			Return (0x0100)  // Interface version 1.0
		}

		Method (MHKA, 0, NotSerialized)  // Hotkey all mask
		{
			Return (0xFFFFFFFF)
		}

		Method (MHKM, 2, NotSerialized)  // Set hotkey mask
		{
			Acquire (XDHK, 0xFFFF)

			Local0 = 1 << Arg0--
			If (Arg1) {
				DHKN |= Local0
			} Else {
				DHKN &= ~Local0
			}

			Release (XDHK)
		}

		Method (MHKC, 1, NotSerialized)  // Hotkey disable?
		{
			Acquire (XDHK, 0xFFFF)

			DHKC = Arg0

			Release (XDHK)
		}

		Method (MHKP, 0, NotSerialized)  // Hotkey poll
		{
			Acquire (XDHK, 0xFFFF)

			Local0 = 0

			if (DHKV) {
				Local0 = DHKV
				DHKV = 0
			}

			Release (XDHK)

			Return (Local0)
		}

		Method (MHKQ, 1, NotSerialized)  // Send hotkey event
		{
			Acquire (XDHK, 0xFFFF)

			DHKV = Arg0

			Release (XDHK)

			Notify (HKEY, 0x80) // Status Change
		}
	}

	/* LED support for thinkpad-acpi */
	Method (LED, 2, NotSerialized)
	{
		HLCL = Arg0 | Arg1
	}

	Device (AC)
	{
		Name (_HID, "ACPI0003")
		Name (_PCL, Package (1) { \_SB })

		Method (_PSR, 0, NotSerialized)
		{
			Return (ONAC)
		}
		Method (_STA, 0, NotSerialized)
		{
			Return (0x0F)
		}
	}

	/* Battery is H8 compatible, but requires an explicit delay */
	#define BATTERY_PAGE_DELAY_MS 20
	#include <ec/lenovo/h8/acpi/battery.asl>

	/* LID is H8 compatible */
	#include <ec/lenovo/h8/acpi/lid.asl>
}
