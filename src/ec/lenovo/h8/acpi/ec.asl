/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include "smi.h"
Device(EC)
{
	Name (_HID, EISAID("PNP0C09"))
	Name (_UID, 0)

	Name (_GPE, 0x11)
	Mutex (ECLK, 0)
	Name (BTN, 0)

	OperationRegion(ERAM, EmbeddedControl, 0x00, 0x100)
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset (0x05),
				HSPA, 1,
		Offset (0x0C),
				LEDS, 8,	/* LED state */
		Offset (0x2a),
				EVNT, 8,	/* write will trigger EC event */
		Offset (0x3a),
				AMUT, 1,	/* Audio Mute */
		Offset (0x3B),
				    , 1,
				KBLT, 1,        /* Keyboard Light */
				    , 2,
				USPW, 1,	/* USB Power enable */
		Offset (0x4e),
		               WAKE, 16,
		Offset (0x78),
				TMP0, 8,	/* Thermal Zone 0 temperature */
				TMP1, 8,	/* Thermal Zone 1 temperature */
		Offset (0x81),
				PAGE, 8		/* Information Page Selector */
        }

	Method (_CRS, 0)
	{
		Name (ECMD, ResourceTemplate()
		{
			IO (Decode16, 0x62, 0x62, 1, 1)
			IO (Decode16, 0x66, 0x66, 1, 1)
		})
		Return (ECMD)
	}

	Method (LED, 1, NotSerialized)
	{
		Store(Arg0, LEDS)
	}

	Method (_INI, 0, NotSerialized)
	{
	}

	Method (MUTE, 1, NotSerialized)
	{
		Store(Arg0, AMUT)
	}

	Method (USBP, 1, NotSerialized)
	{
		Store(Arg0, USPW)
	}


	/* Sleep Button pressed */
	Method(_Q13, 0, NotSerialized)
	{
		Notify(\_SB.PCI0.LPCB.EC.SLPB, 0x80)
	}

	/* Brightness up GPE */
	Method(_Q14, 0, NotSerialized)
	{
                Notify (\_SB.PCI0.GFX0.LCD0, 0x86)
	}

	/* Brightness down GPE */
	Method(_Q15, 0, NotSerialized)
	{
                Notify (\_SB.PCI0.GFX0.LCD0, 0x87)
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
	}

	/* AC status change: not present */
	Method(_Q27, 0, NotSerialized)
	{
		Notify (AC, 0x80)
		Store(0x50, EVNT)
	}

       Method(_Q2A, 0, NotSerialized)
       {
               Notify(\_SB.PCI0.LPCB.EC.LID, 0x80)
       }

       Method(_Q2B, 0, NotSerialized)
       {
               Notify(\_SB.PCI0.LPCB.EC.LID, 0x80)
       }


       /* IBM proprietary buttons.  */

       Method (_Q10, 0, NotSerialized)
       {
            Store (0x1001, BTN)
       }

       Method (_Q11, 0, NotSerialized)
       {
            Store (0x1002, BTN)
       }

       Method (_Q12, 0, NotSerialized)
       {
            Store (0x1003, BTN)
       }

       Method (_Q64, 0, NotSerialized)
       {
            Store (0x1005, BTN)
       }

       Method (_Q65, 0, NotSerialized)
       {
            Store (0x1006, BTN)
       }

       Method (_Q17, 0, NotSerialized)
       {
            Store (0x1008, BTN)
       }

       Method (_Q66, 0, NotSerialized)
       {
            Store (0x100A, BTN)
       }

       Method (_Q1A, 0, NotSerialized)
       {
            Store (0x100B, BTN)
       }

       Method (_Q1B, 0, NotSerialized)
       {
            Store (0x100C, BTN)
       }

       Method (_Q62, 0, NotSerialized)
       {
            Store (0x100D, BTN)
       }

       Method (_Q60, 0, NotSerialized)
       {
            Store (0x100E, BTN)
       }

       Method (_Q61, 0, NotSerialized)
       {
            Store (0x100F, BTN)
       }

       Method (_Q1F, 0, NotSerialized)
       {
            Store (0x1012, BTN)
       }

       Method (_Q67, 0, NotSerialized)
       {
            Store (0x1013, BTN)
       }

       Method (_Q63, 0, NotSerialized)
       {
            Store (0x1014, BTN)
       }

       Method (_Q19, 0, NotSerialized)
       {
            Store (0x1018, BTN)
       }

       Method (_Q1C, 0, NotSerialized)
       {
            Store (0x1019, BTN)
       }

       Method (_Q1D, 0, NotSerialized)
       {
            Store (0x101A, BTN)
       }

       Device (HKEY)
       {
               Name (_HID, EisaId ("IBM0068"))
               Method (_STA, 0, NotSerialized)
               {
                    Return (0x0F)
               }
               Method (MHKP, 0, NotSerialized)
               {
	            Store (^^BTN, Local0)
		    If (LEqual (Local0, Zero)) {
		       Return (Zero)
                    }
		    Store (Zero, ^^BTN)
		    Return (Local0)
               }
       }

#include "ac.asl"
#include "battery.asl"
#include "sleepbutton.asl"
#include "lid.asl"
#include "beep.asl"
#include "thermal.asl"
#include "systemstatus.asl"
}
