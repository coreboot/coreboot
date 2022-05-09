/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/amd/common/acpi/aoac.asl>
#include <soc/aoac_defs.h>
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

	Method (_CRS, 0) {
		Local0 = ResourceTemplate() {
			Interrupt (
				ResourceConsumer,
				Level,
				ActiveLow,
				Shared, , , IRQR)
			{ 0 }
			Memory32Fixed (ReadWrite, ACPIMMIO_GPIO0_BASE, 0x400)
		}
		CreateDWordField (Local0, IRQR._INT, IRQN)
		If (PICM) {
			IRQN = IGPI
		} Else {
			IRQN = PGPI
		}
		If (IRQN == 0x1f) {
			Return (ResourceTemplate() {
				Memory32Fixed (ReadWrite, ACPIMMIO_GPIO0_BASE, 0x400)
			})
		} Else {
			Return (Local0)
		}
	}

	Method (_STA, 0x0, NotSerialized)
	{
		Return (0x0F)
	}
}

Device (MMC0)
{
	Name (_HID, "AMDI0040")
	Name (_UID, 0x0)
	Method (_CRS, 0) {
		Local0 = ResourceTemplate() {
			Interrupt (
				ResourceConsumer,
				Level,
				ActiveLow,
				Exclusive, , , IRQR)
			{ 0 }
			Memory32Fixed (ReadWrite, APU_EMMC_BASE, 0x1000)
		}
		CreateDWordField (Local0, IRQR._INT, IRQN)
		If (PICM) {
			IRQN = IMMC
		} Else {
			IRQN = PMMC
		}
		If (IRQN == 0x1f) {
			Return (ResourceTemplate(){
				Memory32Fixed (ReadWrite, APU_EMMC_BASE, 0x1000)
			})
		} Else {
			Return (Local0)
		}
	}

	Method (_STA, 0x0, NotSerialized)
	{
		Return (0x0F)
	}
}

Device (FUR0)
{
	Name (_HID, "AMDI0020")
	Name (_UID, 0x0)
	Method (_CRS, 0) {
		Local0 = ResourceTemplate() {
			Interrupt (
				ResourceConsumer,
				Edge,
				ActiveHigh,
				Exclusive, , , IRQR)
			{ 0 }
			Memory32Fixed (ReadWrite, APU_UART0_BASE, 0x1000)
		}
		CreateDWordField (Local0, IRQR._INT, IRQN)
		If (PICM) {
			IRQN = IUA0
		} Else {
			IRQN = PUA0
		}
		If (IRQN == 0x1f) {
			Return (ResourceTemplate() {
				Memory32Fixed (ReadWrite, APU_UART0_BASE, 0x1000)
			})
		} Else {
			Return (Local0)
		}
	}

	AOAC_DEVICE(FCH_AOAC_DEV_UART0, 0)
}

Device (FUR1) {
	Name (_HID, "AMDI0020")
	Name (_UID, 0x1)
	Method (_CRS, 0) {
		Local0 = ResourceTemplate() {
			Interrupt (
				ResourceConsumer,
				Edge,
				ActiveHigh,
				Exclusive, , , IRQR)
			{ 0 }
			Memory32Fixed (ReadWrite, APU_UART1_BASE, 0x1000)
		}
		CreateDWordField (Local0, IRQR._INT, IRQN)
		If (PICM) {
			IRQN = IUA1
		} Else {
			IRQN = PUA1
		}
		If (IRQN == 0x1f) {
			Return (ResourceTemplate() {
				Memory32Fixed (ReadWrite, APU_UART1_BASE, 0x1000)
			})
		} Else {
			Return (Local0)
		}
	}

	AOAC_DEVICE(FCH_AOAC_DEV_UART1, 0)
}

Device (FUR2) {
	Name (_HID, "AMDI0020")
	Name (_UID, 0x2)
	Method (_CRS, 0) {
		Local0 = ResourceTemplate() {
			Interrupt (
				ResourceConsumer,
				Edge,
				ActiveHigh,
				Exclusive, , , IRQR)
			{ 0 }
			Memory32Fixed (ReadWrite, APU_UART2_BASE, 0x1000)
		}
		CreateDWordField (Local0, IRQR._INT, IRQN)
		If (PICM) {
			IRQN = IUA2
		} Else {
			IRQN = PUA2
		}
		If (IRQN == 0x1f) {
			Return (ResourceTemplate() {
				Memory32Fixed (ReadWrite, APU_UART2_BASE, 0x1000)
			})
		} Else {
			Return (Local0)
		}
	}

	AOAC_DEVICE(FCH_AOAC_DEV_UART2, 0)
}

Device (FUR3) {
	Name (_HID, "AMDI0020")
	Name (_UID, 0x3)
	Method (_CRS, 0) {
		Local0 = ResourceTemplate() {
			Interrupt (
				ResourceConsumer,
				Edge,
				ActiveHigh,
				Exclusive, , , IRQR)
			{ 0 }
			Memory32Fixed (ReadWrite, APU_UART3_BASE, 0x1000)
		}
		CreateDWordField (Local0, IRQR._INT, IRQN)
		If (PICM) {
			IRQN = IUA3
		} Else {
			IRQN = PUA3
		}
		If (IRQN == 0x1f) {
			Return (ResourceTemplate() {
				Memory32Fixed (ReadWrite, APU_UART3_BASE, 0x1000)
			})
		} Else {
			Return (Local0)
		}
	}

	AOAC_DEVICE(FCH_AOAC_DEV_UART3, 0)
}

Device (I2C2) {
	Name (_HID, "AMDI0010")
	Name (_UID, 0x2)
	Method (_CRS, 0) {
		Local0 = ResourceTemplate() {
			Interrupt (
				ResourceConsumer,
				Edge,
				ActiveHigh,
				Exclusive, , , IRQR)
			{ 0 }
			Memory32Fixed (ReadWrite, APU_I2C2_BASE, 0x1000)
		}
		CreateDWordField (Local0, IRQR._INT, IRQN)
		If (PICM) {
			IRQN = II22
		} Else {
			IRQN = PI22
		}
		If (IRQN == 0x1f) {
			Return (ResourceTemplate() {
				Memory32Fixed (ReadWrite, APU_I2C2_BASE, 0x1000)
			})
		} Else {
			Return (Local0)
		}
	}

	Method (_STA, 0x0, NotSerialized)
	{
		Return (0x0F)
	}

	AOAC_DEVICE(FCH_AOAC_DEV_I2C2, 0)
}

Device (I2C3)
{
	Name (_HID, "AMDI0010")
	Name (_UID, 0x3)
	Method (_CRS, 0) {
		Local0 = ResourceTemplate() {
			Interrupt (
				ResourceConsumer,
				Edge,
				ActiveHigh,
				Exclusive, , , IRQR)
			{ 0 }
			Memory32Fixed (ReadWrite, APU_I2C3_BASE, 0x1000)
		}
		CreateDWordField (Local0, IRQR._INT, IRQN)
		If (PICM) {
			IRQN = II23
		} Else {
			IRQN = PI23
		}
		If (IRQN == 0x1f) {
			Return (ResourceTemplate() {
				Memory32Fixed (ReadWrite, APU_I2C3_BASE, 0x1000)
			})
		} Else {
			Return (Local0)
		}
	}
	Method (_STA, 0x0, NotSerialized)
	{
		Return (0x0F)
	}

	AOAC_DEVICE(FCH_AOAC_DEV_I2C3, 0)
}

Device (MISC)
{
	Name (_HID, "AMD0040")
	Name (_UID, 0x3)
	Name (_CRS, ResourceTemplate() {
		Memory32Fixed (ReadWrite, ACPIMMIO_MISC_BASE, 0x100)
	})
	Name (_DSD, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package () { "is-rv", 1 },
		},
	})
	Method (_STA, 0x0, NotSerialized)
	{
		Return (0x0F)
	}
}
