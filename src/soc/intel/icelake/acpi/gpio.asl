/* SPDX-License-Identifier: GPL-2.0-only */
#include <soc/intel/common/block/acpi/acpi/gpio_op.asl>
#include <soc/gpio_defs.h>
#include <soc/irq.h>
#include <soc/pcr_ids.h>

Device (GPIO)
{
	Name (_HID, "INT3455")
	Name (_UID, 0)
	Name (_DDN, "GPIO Controller")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0, COM0)
		Memory32Fixed (ReadWrite, 0, 0, COM1)
		Memory32Fixed (ReadWrite, 0, 0, COM2)
		Memory32Fixed (ReadWrite, 0, 0, COM4)
		Memory32Fixed (ReadWrite, 0, 0, COM5)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ)
			{ GPIO_IRQ14 }
	})

	Method (_CRS, 0, NotSerialized)
	{
		/* GPIO Community 0 */
		CreateDWordField (^RBUF, ^COM0._BAS, BAS0)
		CreateDWordField (^RBUF, ^COM0._LEN, LEN0)
		BAS0 = ^^PCRB (PID_GPIOCOM0)
		LEN0 = GPIO_BASE_SIZE

		/* GPIO Community 1 */
		CreateDWordField (^RBUF, ^COM1._BAS, BAS1)
		CreateDWordField (^RBUF, ^COM1._LEN, LEN1)
		BAS1 = ^^PCRB (PID_GPIOCOM1)
		LEN1 = GPIO_BASE_SIZE

		/* GPIO Community 2 */
		CreateDWordField (^RBUF, ^COM2._BAS, BAS2)
		CreateDWordField (^RBUF, ^COM2._LEN, LEN2)
		BAS2 = ^^PCRB (PID_GPIOCOM2)
		LEN2 = GPIO_BASE_SIZE

		/* GPIO Community 4 */
		CreateDWordField (^RBUF, ^COM4._BAS, BAS4)
		CreateDWordField (^RBUF, ^COM4._LEN, LEN4)
		BAS4 = ^^PCRB (PID_GPIOCOM4)
		LEN4 = GPIO_BASE_SIZE

		/* GPIO Community 5 */
		CreateDWordField (^RBUF, ^COM5._BAS, BAS5)
		CreateDWordField (^RBUF, ^COM5._LEN, LEN5)
		BAS5 = ^^PCRB (PID_GPIOCOM5)
		LEN5 = GPIO_BASE_SIZE

		Return (RBUF)
	}

	Method (_STA, 0, NotSerialized)
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
	If ((Arg0 >= GPP_G0) && (Arg0 <= GPP_A23))
	{
		Local0 = PID_GPIOCOM0
		Local1 = Arg0 - GPP_G0
	}
	/* GPIO Community 1 */
	If ((Arg0 >= GPP_H0) && (Arg0 <= GPP_F19))
	{
		Local0 = PID_GPIOCOM1
		Local1 = Arg0 - GPP_H0
	}
	/* GPIO Community 2 */
	If ((Arg0 >= GPD0) && (Arg0 <= GPD11))
	{
		Local0 = PID_GPIOCOM2
		Local1 = Arg0 - GPD0
	}
	/* GPIO Community 4 */
	If ((Arg0 >= GPP_C0) && (Arg0 <= GPP_E23))
	{
		Local0 = PID_GPIOCOM4
		Local1 = Arg0 - GPP_C0
	}
	/* GPIO Community 5 */
	If ((Arg0 >= GPP_R0) && (Arg0 <= GPP_S7))
	{
		Local0 = PID_GPIOCOM5
		Local1 = Arg0 - GPP_R0
	}

	Local2 = PCRB (Local0)
	Local2 += PAD_CFG_BASE
	Return (Local2 + (Local1 * 16))
}
