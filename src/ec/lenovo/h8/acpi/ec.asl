/* SPDX-License-Identifier: GPL-2.0-only */

Device(EC)
{
	Name (_HID, EISAID("PNP0C09"))
	Name (_UID, 0)

	Name (_GPE, THINKPAD_EC_GPE)
	Mutex (ECLK, 0)

	OperationRegion(ERAM, EmbeddedControl, 0x00, 0x100)
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0x02),
				DKR1, 1,	/* Dock register 1 */
		Offset (0x05),
				HSPA, 1,
		Offset (0x0C),
				LEDS, 8,	/* LED state */
		Offset (0x0F),
				    , 7,
				TBSW, 1,	/* Tablet mode switch */
		Offset (0x1a),
				DKR2, 1,	/* Dock register 2 */
		Offset (0x2a),
				EVNT, 8,	/* write will trigger EC event */
		Offset (0x2f),
				    , 6,
				FAND, 1,	/* Fan disengage */
				FANA, 1,	/* Fan automatic mode enable */
		Offset (0x30),
				    , 6,
				ALMT, 1,	/* Audio Mute + LED */
		Offset (0x31),
				    , 2,
				UWBE, 1,	/* Ultra Wideband enable */
		Offset (0x3a),
				AMUT, 1,	/* Audio Mute (internal use) */
				    , 3,
				BTEB, 1,
				WLEB, 1,
				WWEB, 1,
		Offset (0x3B),
				    , 1,
				KBLT, 1,	/* Keyboard Light */
				    , 2,
				USPW, 1,	/* USB Power enable */
		Offset (0x48),
				HPPI, 1,	/* Headphone plugged in */
				GSTS, 1,	/* Master wireless switch */
		Offset (0x4e),
				WAKE, 16,
		Offset (0x78),
				TMP0, 8,	/* Thermal Zone 0 temperature */
				TMP1, 8,	/* Thermal Zone 1 temperature */
		Offset (0x81),
				PAGE, 8,	/* Information Page Selector */
		Offset (0xfe),
				    , 4,
				DKR3, 1		/* Dock register 3 */
	}

	/* Called on OperationRegion driver changes */
	Method (_REG, 2, NotSerialized)
	{
		/* Wait for ERAM driver loaded */
		if (Arg1 == 1) {
			/* Fill HKEY defaults on first boot */
			if (^HKEY.INIT == 0) {
				^HKEY.WBDC = BTEB
				^HKEY.WWAN = WWEB
				^HKEY.INIT = One
			}
		}
	}

	Method (_CRS, 0, Serialized)
	{
		Name (ECMD, ResourceTemplate()
		{
			IO (Decode16, 0x62, 0x62, 1, 1)
			IO (Decode16, 0x66, 0x66, 1, 1)
		})
		Return (ECMD)
	}

	Method (TLED, 1, NotSerialized)
	{
		LEDS = Arg0
	}

	/* Not used for coreboot. Provided for compatibility with thinkpad-acpi.  */
	Method (LED, 2, NotSerialized)
	{
		TLED(Or(Arg0, Arg1))
	}

	Method (_INI, 0, NotSerialized)
	{
	}

	Method (MUTE, 1, NotSerialized)
	{
		AMUT = Arg0
	}

	Method (RADI, 1, NotSerialized)
	{
		WLEB = Arg0
		WWEB = Arg0
		BTEB = Arg0
	}

	Method (USBP, 1, NotSerialized)
	{
		USPW = Arg0
	}

	Method (LGHT, 1, NotSerialized)
	{
		KBLT = Arg0
	}


	/* Sleep Button pressed */
	Method(_Q13, 0, NotSerialized)
	{
		Notify(^SLPB, 0x80)
	}

	/* Brightness up GPE */
	Method(_Q14, 0, NotSerialized)
	{
		BRIGHTNESS_UP()
	}

	/* Brightness down GPE */
	Method(_Q15, 0, NotSerialized)
	{
		BRIGHTNESS_DOWN()
	}

	/* Next display GPE */
	Method(_Q16, 0, NotSerialized)
	{
		Notify (\_SB.PCI0.GFX0, 0x82)
	}

	/* AC status change: present */
	Method(_Q26, 0, NotSerialized)
	{
		Notify (AC, 0x80)
		\PNOT()
	}

	/* AC status change: not present */
	Method(_Q27, 0, NotSerialized)
	{
		Notify (AC, 0x80)
		EVNT = 0x50
		\PNOT()
	}

	Method(_Q2A, 0, NotSerialized)
	{
		Notify(^LID, 0x80)
	}

	Method(_Q2B, 0, NotSerialized)
	{
		Notify(^LID, 0x80)
	}


	/* IBM proprietary buttons.  */

	Method (_Q10, 0, NotSerialized)
	{
		^HKEY.RHK (0x01)
	}

	/*
	 * Alternative layout (like in the Thinkpad X1 Carbon 1st generation):
	 *  * Fn-F2 (_Q11) -> not mapped
	 *  * Fn-F3 (_Q12) -> scancode 0x01 (KEY_COFFEE)
	 *
	 * Default layout (like in the Thinkpad X220):
	 *  * Fn-F2 (_Q11) -> scancode 0x01 (KEY_COFFEE)
	 *  * Fn-F3 (_Q12) -> scancode 0x02 (KEY_BATTERY)
	 */
#ifdef EC_LENOVO_H8_ALT_FN_F2F3_LAYOUT
	Method (_Q11, 0, NotSerialized)
	{
		// Not mapped
	}

	Method (_Q12, 0, NotSerialized)
	{
		^HKEY.RHK (0x02)
	}
#else
	Method (_Q11, 0, NotSerialized)
	{
		^HKEY.RHK (0x02)
	}

	Method (_Q12, 0, NotSerialized)
	{
		^HKEY.RHK (0x03)
	}
#endif

	Method (_Q64, 0, NotSerialized)
	{
		^HKEY.RHK (0x05)
	}

	Method (_Q65, 0, NotSerialized)
	{
		^HKEY.RHK (0x06)
	}

	Method (_Q17, 0, NotSerialized)
	{
		^HKEY.RHK (0x08)
	}

	Method (_Q66, 0, NotSerialized)
	{
		^HKEY.RHK (0x0A)
	}

	Method (_Q6A, 0, NotSerialized)
	{
		^HKEY.RHK (0x1B)
	}

	Method (_Q1A, 0, NotSerialized)
	{
		^HKEY.RHK (0x0B)
	}

	Method (_Q1B, 0, NotSerialized)
	{
		^HKEY.RHK (0x0C)
	}

	Method (_Q62, 0, NotSerialized)
	{
		^HKEY.RHK (0x0D)
	}

	Method (_Q60, 0, NotSerialized)
	{
		^HKEY.RHK (0x0E)
	}

	Method (_Q61, 0, NotSerialized)
	{
		^HKEY.RHK (0x0F)
	}

	Method (_Q1F, 0, NotSerialized)
	{
		^HKEY.RHK (0x12)
	}

	Method (_Q67, 0, NotSerialized)
	{
		^HKEY.RHK (0x13)
	}

	Method (_Q63, 0, NotSerialized)
	{
		^HKEY.RHK (0x14)
	}

	Method (_Q19, 0, NotSerialized)
	{
		^HKEY.RHK (0x18)
	}

	Method (_Q1C, 0, NotSerialized)
	{
		^HKEY.RHK (0x19)
	}

	Method (_Q1D, 0, NotSerialized)
	{
		^HKEY.RHK (0x1A)
	}

	Method (_Q5C, 0, NotSerialized)
	{
		^HKEY.RTAB (0xB)
	}

	Method (_Q5D, 0, NotSerialized)
	{
		^HKEY.RTAB (0xC)
	}

	Method (_Q5E, 0, NotSerialized)
	{
		^HKEY.RTAB (0x9)
	}

	Method (_Q5F, 0, NotSerialized)
	{
		^HKEY.RTAB (0xA)
	}

	/*
	 * Set FAN disengage:
	 * Arg0: 1: Run at full speed
	 *       0: Automatic fan control
	 */
	Method (FANE, 1, Serialized)
	{
		If (Arg0) {
			FAND = One
			FANA = Zero
		} Else {
			FAND = Zero
			FANA = One
		}
	}

#include "ac.asl"
#include "battery.asl"
#include "sleepbutton.asl"
#include "lid.asl"
#include "beep.asl"
#include "thermal.asl"
#include "systemstatus.asl"
#include "thinkpad.asl"
}
