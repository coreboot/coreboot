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

	Name (_GPE, 28)
	Mutex (ECLK, 0)

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
		\DSPC.BRTU ()
	}

	/* Brightness down GPE */
	Method(_Q15, 0, NotSerialized)
	{
		\DSPC.BRTD()
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


#include "ac.asl"
#include "battery.asl"
#include "sleepbutton.asl"
#include "lid.asl"
#include "beep.asl"
#include "thermal.asl"
#include "systemstatus.asl"
}
