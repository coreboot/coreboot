/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <soc/gpio.h>
#include <soc/iomap.h>

Device (AAHB)
{
	Name (_HID, "AAHB0000")
	Name (_UID, 0x0)
	Name (_CRS, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0xFEDC0000, 0x2000)
	})

	Method (_STA, 0x0, NotSerialized)
	{
		Return (0x0F)
	}
}

Device (GPIO)
{
	Name (_HID, GPIO_DEVICE_NAME)
	Name (_CID, GPIO_DEVICE_NAME)
	Name (_UID, 0)
	Name (_DDN, GPIO_DEVICE_DESC)

	Name (_CRS, ResourceTemplate()
	{
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , )
			{ 7 }
		Memory32Fixed (ReadWrite, 0xFED81500, 0x300)
	})

	Method (_STA, 0x0, NotSerialized)
	{
		Return (0x0F)
	}
}

Device (FUR0)
{
	Name (_HID, "AMD0020")
	Name (_UID, 0x0)
	Name (_CRS, ResourceTemplate()
	{
		IRQ (Edge, ActiveHigh, Exclusive) { 10 }
		Memory32Fixed (ReadWrite, 0xFEDC6000, 0x2000)
	})
	Method (_STA, 0x0, NotSerialized)
	{
		Return (0x0F)
	}
}

Device (FUR1) {
	Name (_HID, "AMD0020")
	Name (_UID, 0x1)
	Name (_CRS, ResourceTemplate()
	{
			IRQ (Edge, ActiveHigh, Exclusive) { 11 }
			Memory32Fixed (ReadWrite, 0xFEDC8000, 0x2000)
	})
	Method (_STA, 0x0, NotSerialized)
	{
			Return (0x0F)
	}
}

Device (I2CA) {
	Name (_HID, "AMD0010")
	Name (_UID, 0x0)
	Name (_CRS, ResourceTemplate()
	{
		IRQ (Edge, ActiveHigh, Exclusive) { 3 }
		Memory32Fixed (ReadWrite, 0xFEDC2000, 0x1000)
	})

	Method (_STA, 0x0, NotSerialized)
	{
		Return (0x0F)
	}
}

Device (I2CB)
{
	Name (_HID, "AMD0010")
	Name (_UID, 0x1)
	Name (_CRS, ResourceTemplate()
	{
		IRQ (Edge, ActiveHigh, Exclusive) { 15 }
		Memory32Fixed (ReadWrite, 0xFEDC3000, 0x1000)
	})
	Method (_STA, 0x0, NotSerialized)
	{
		Return (0x0F)
	}
}

Device (I2CC) {
	Name (_HID, "AMD0010")
	Name (_UID, 0x2)
	Name (_CRS, ResourceTemplate()
	{
		IRQ (Edge, ActiveHigh, Exclusive) { 6 }
		Memory32Fixed (ReadWrite, 0xFEDC4000, 0x1000)
	})

	Method (_STA, 0x0, NotSerialized)
	{
		Return (0x0F)
	}
}

Device (I2CD)
{
	Name (_HID, "AMD0010")
	Name (_UID, 0x3)
	Name (_CRS, ResourceTemplate() {
		IRQ (Edge, ActiveHigh, Exclusive) { 14 }
		Memory32Fixed(ReadWrite, 0xFEDC5000, 0x1000)
	})
	Method (_STA, 0x0, NotSerialized)
	{
		Return (0x0F)
	}
}

Device (MISC)
{
	Name (_HID, "AMD0040")
	Name (_UID, 0x3)
	Name (_CRS, ResourceTemplate() {
		Memory32Fixed(ReadWrite, MISC_MMIO_BASE, 0x100)
	})
	Method (_STA, 0x0, NotSerialized)
	{
		Return (0x0F)
	}
}
