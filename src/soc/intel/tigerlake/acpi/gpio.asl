/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */
#include <soc/gpio_defs.h>
#include <soc/irq.h>
#include <soc/pcr_ids.h>
#include <intelblocks/gpio.h>
#include "gpio_op.asl"

Device (GCM0)
{
	Name (_HID, CROS_GPIO_NAME)
	Name (_UID, 0)
	Name (_DDN, "GPIO Controller Community 0")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, GPIO_BASE_SIZE, COM0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ)
			{ GPIO_IRQ14 }
	})
	Method (_CRS, 0, NotSerialized)
	{
		CreateDWordField (^RBUF, ^COM0._BAS, BAS0)
		BAS0 = ^^PCRB (PID_GPIOCOM0)
		Return (^RBUF)
	}
	Method (_STA)
	{
		Return (0xF)
	}
}

Device (GCM1)
{
	Name (_HID, CROS_GPIO_NAME)
	Name (_UID, 1)
	Name (_DDN, "GPIO Controller Community 1")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, GPIO_BASE_SIZE, COM1)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ)
			{ GPIO_IRQ14 }
	})
	Method (_CRS, 0, NotSerialized)
	{
		CreateDWordField (^RBUF, ^COM1._BAS, BAS1)
		BAS1 = ^^PCRB (PID_GPIOCOM1)
		Return (^RBUF)
	}
	Method (_STA)
	{
		Return (0xF)
	}
}

Device (GCM4)
{
	Name (_HID, CROS_GPIO_NAME)
	Name (_UID, 4)
	Name (_DDN, "GPIO Controller Community 4")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, GPIO_BASE_SIZE, COM4)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ)
			{ GPIO_IRQ14 }
	})
	Method (_CRS, 0, NotSerialized)
	{
		CreateDWordField (^RBUF, ^COM4._BAS, BAS4)
		BAS4 = ^^PCRB (PID_GPIOCOM4)
		Return (^RBUF)
	}
	Method (_STA)
	{
		Return (0xF)
	}
}

Device (GCM5)
{
	Name (_HID, CROS_GPIO_NAME)
	Name (_UID, 5)
	Name (_DDN, "GPIO Controller Community 5")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, GPIO_BASE_SIZE, COM5)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ)
		{ GPIO_IRQ14 }
	})
	Method (_CRS, 0, NotSerialized)
	{
		CreateDWordField (^RBUF, ^COM5._BAS, BAS5)
		BAS5 = ^^PCRB (PID_GPIOCOM5)
		Return (^RBUF)
	}
	Method (_STA)
	{
		Return (0xF)
	}
}

/*
 * Get GPIO DW0 Address
 * Arg0 - GPIO Number
 */
Method (GADD, 1, NotSerialized)
{
	/* GPIO Community 0 */
	If (Arg0 >= GPIO_COM0_START && Arg0 <= GPIO_COM0_END)
	{
		Local0 = PID_GPIOCOM0
		Local1 = Arg0 - GPIO_COM0_START
	}
	/* GPIO Community 1 */
	If (Arg0 >= GPIO_COM1_START && Arg0 <= GPIO_COM1_END)
	{
		Local0 = PID_GPIOCOM1
		Local1 = Arg0 - GPIO_COM1_START
	}
	/* GPIO Community 2 */
	If (Arg0 >= GPIO_COM2_START && Arg0 <= GPIO_COM2_END)
	{
		Local0 = PID_GPIOCOM2
		Local1 = Arg0 - GPIO_COM2_START
	}
	/* GPIO Community 4 */
	If (Arg0 >= GPIO_COM4_START && Arg0 <= GPIO_COM4_END)
	{
		Local0 = PID_GPIOCOM4
		Local1 = Arg0 - GPIO_COM4_START
	}
	/* GPIO Community 05*/
	If (Arg0 >= GPIO_COM5_START && Arg0 <= GPIO_COM5_END)
	{
		Local0 = PID_GPIOCOM5
		Local1 = Arg0 - GPIO_COM5_START
	}

	Local2 = PCRB(Local0) + PAD_CFG_BASE + (Local1 * 16)
	Return (Local2)
}
