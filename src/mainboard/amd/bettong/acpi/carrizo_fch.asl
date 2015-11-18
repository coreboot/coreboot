/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

Device(GPIO) {
	Name (_HID, "AMD0030")
	Name (_CID, "AMD0030")
	Name(_UID, 0)

	Method (_CRS, 0x0, NotSerialized) {
		Name (RBUF, ResourceTemplate () {
			//
			// Interrupt resource. In this example, banks 0 & 1 share the same
			// interrupt to the parent controller and similarly banks 2 & 3.
			//
			// N.B. The definition below is chosen for an arbitrary
			//      test platform. It needs to be changed to reflect the hardware
			//      configuration of the actual platform
			//
			Interrupt(ResourceConsumer, Level, ActiveLow, Shared, , , ) {7}

			//
			// Memory resource. The definition below is chosen for an arbitrary
			// test platform. It needs to be changed to reflect the hardware
			// configuration of the actual platform.
			//
			Memory32Fixed(ReadWrite, 0xFED81500, 0x300)
		})

		Return (RBUF)
	}

	Method (_STA, 0x0, NotSerialized) {
			Return (0x0F)
	}
}

Device(FUR0) {
	Name(_HID,"AMD0020")
	Name(_UID,0x0)
	Name(_CRS, ResourceTemplate() {
		IRQ(Edge, ActiveHigh, Exclusive) {10}
		Memory32Fixed(ReadWrite, 0xFEDC6000, 0x2000)
	})
	Method (_STA, 0x0, NotSerialized) {
		Return (0x0F)
	}
}

Device(FUR1) {
	Name(_HID,"AMD0020")
	Name(_UID,0x1)
	Name(_CRS, ResourceTemplate() {
			IRQ(Edge, ActiveHigh, Exclusive) {11}
			Memory32Fixed(ReadWrite, 0xFEDC8000, 0x2000)
	})
	Method (_STA, 0x0, NotSerialized) {
			Return (0x0F)
	}
}

Device(I2CA) {
	Name(_HID,"AMD0010")
	Name(_UID,0x0)
	Name(_CRS, ResourceTemplate() {
		IRQ(Edge, ActiveHigh, Exclusive) {3}
		Memory32Fixed(ReadWrite, 0xFEDC2000, 0x1000)
	})

	Method (_STA, 0x0, NotSerialized) {
		Return (0x0F)
	}
}

Device(I2CB)
{
	Name(_HID,"AMD0010")
	Name(_UID,0x1)
	Name(_CRS, ResourceTemplate() {
		IRQ(Edge, ActiveHigh, Exclusive) {15}
		Memory32Fixed(ReadWrite, 0xFEDC3000, 0x1000)
	})
	Method (_STA, 0x0, NotSerialized) {
		Return (0x0F)
	}
}
