/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>
#include <soc/iomap.h>
#include <amdblocks/acpimmio_map.h>

Device (AAHB)
{
	Name (_HID, "AAHB0000")
	Name (_UID, 0x0)
	Name (_CRS, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, ALINK_AHB_ADDRESS, 0x2000)
	})
	Name (_STA, 0xb)
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
		Memory32Fixed (ReadWrite, ACPIMMIO_GPIO0_BASE, 0x300)
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
		Memory32Fixed (ReadWrite, APU_UART0_BASE, 0x2000)
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
			Memory32Fixed (ReadWrite, APU_UART1_BASE, 0x2000)
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
		Memory32Fixed (ReadWrite, APU_I2C0_BASE, 0x1000)
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
		Memory32Fixed (ReadWrite, APU_I2C1_BASE, 0x1000)
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
		Memory32Fixed (ReadWrite, APU_I2C2_BASE, 0x1000)
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
		Memory32Fixed(ReadWrite, APU_I2C3_BASE, 0x1000)
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
		Memory32Fixed(ReadWrite, ACPIMMIO_MISC_BASE, 0x100)
	})
	Method (_STA, 0x0, NotSerialized)
	{
		Return (0x0F)
	}
}
