/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2012 Google Inc.
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
 */

#include <mainboard/google/parrot/onboard.h>

Scope (\_GPE) {
	Method(_L1F, 0x0, NotSerialized)
	{
		/*
		 * Invert the interrupt level bit for the lid GPIO
		 * so we don't get another _SB.LID0 until the state
		 * changes again. GIV1 is the interrupt level control
		 * register for GPIO bits 15:8
		 */
		Xor(GIV1, 0x80, GIV1)
		Notify(\_SB.LID0,0x80)
	}
}

Scope (\_SB) {
	Device (LID0)
	{
		Name(_HID, EisaId("PNP0C0D"))
		Method(_LID, 0)
		{
			Store (GP15, \LIDS)
			Return (\LIDS)
		}
	}

	Device (TPAD)
	{
		Name (_ADR, 0x0)
		Name (_UID, 1)

		// Report as a Sleep Button device so Linux will
		// automatically enable it as a wake source
		Name(_HID, EisaId("PNP0C0E"))

		// Trackpad Wake is GPIO12, wake from S3
		Name(_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x03 })

		Name (DCRS, ResourceTemplate ()
		{
			// PIRQA -> GSI16
			Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive, ,, )
			{
				BOARD_TRACKPAD_IRQ_DVT,
			}
			// SMBUS Address 0x67
			VendorShort (ADDR) { BOARD_TRACKPAD_I2C_ADDR }
		})

		Name (PCRS, ResourceTemplate ()
		{
			// PIRQE -> GSI20
			Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive, ,, )
			{
				BOARD_TRACKPAD_IRQ_PVT,
			}
			// SMBUS Address 0x67
			VendorShort (ADDR) { BOARD_TRACKPAD_I2C_ADDR }
		})

		Method (_CRS, 0, NotSerialized)
		{
			If (\TPIQ == BOARD_TRACKPAD_IRQ_DVT){
				Return (DCRS)
			} Else {
				Return (PCRS)
			}
		}
	}

	Device (MB) {
		Name(_HID, EisaId("PNP0C01")) // System Board

		/* Lid open */
		Method (LIDO) { /* Not needed on this board */ }
		/* Lid closed */
		Method (LIDC) { /* Not needed on this board */ }
		/* Increase brightness */
		Method (BRTU) { /* Not needed on this board */ }
		/* Decrease brightness */
		Method (BRTD) { /* Not needed on this board */ }
		/* Switch display */
		Method (DSPS) { /* Not needed on this board */ }
		/* Toggle wireless */
		Method (WLTG) { /* Not needed on this board */ }
		/* Return lid state */
		Method (LIDS)
		{
			Return (GP15)
		}
	}

}

/* USB port entries */
#include "acpi/usb.asl"
