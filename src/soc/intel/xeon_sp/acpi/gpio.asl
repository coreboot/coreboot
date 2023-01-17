/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <soc/pcr_ids.h>
#include <soc/irq.h>
#include <soc/intel/common/block/acpi/acpi/gpio_op.asl>
#include <soc/intel/common/acpi/pcr.asl>

Device (GPIO)
{
	Name (_HID, "INT3536")
	Name (_UID, 0)
	Name (_DDN, "GPIO Controller")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0, COM0)
		Memory32Fixed (ReadWrite, 0, 0, COM1)
		Memory32Fixed (ReadWrite, 0, 0, COM2)
		Memory32Fixed (ReadWrite, 0, 0, COM3)
		Memory32Fixed (ReadWrite, 0, 0, COM4)
		Memory32Fixed (ReadWrite, 0, 0, COM5)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ) { PCH_IRQ14 }
	})

	/* Current Resource Settings */
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

		/* GPIO Community 3 */
		CreateDWordField (^RBUF, ^COM3._BAS, BAS3)
		CreateDWordField (^RBUF, ^COM3._LEN, LEN3)
		BAS3 = ^^PCRB (PID_GPIOCOM3)
		LEN3 = GPIO_BASE_SIZE

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

	/* Return status of power resource */
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
	if ((Arg0 >= GPP_A0) && (Arg0 <= GPP_F23))
	{
		Local0 = PID_GPIOCOM0
		Local1 = Arg0 - GPP_A0
	}

	/* GPIO Community 1 */
	if ((Arg0 >= GPP_C0) && (Arg0 <= GPP_E12))
	{
		Local0 = PID_GPIOCOM1
		Local1 = Arg0 - GPP_C0
	}

	/* GPIO Community 2 */
	if ((Arg0 >= GPD0) && (Arg0 <= GPD11))
	{
		Local0 = PID_GPIOCOM2
		Local1 = Arg0 - GPD0
	}

	/* GPIO Community 3 */
	if ((Arg0 >= GPP_I0) && (Arg0 <= GPP_I10))
	{
		Local0 = PID_GPIOCOM3
		Local1 = Arg0 - GPP_I0
	}

	/* GPIO Community 4 */
	if ((Arg0 >= GPP_J0) && (Arg0 <= GPP_K10))
	{
		Local0 = PID_GPIOCOM4
		Local1 = Arg0 - GPP_J0
	}

	/* GPIO Community 5 */
	if ((Arg0 >= GPP_G0) && (Arg0 <= GPP_L19))
	{
		Local0 = PID_GPIOCOM5
		Local1 = Arg0 - GPP_G0
	}

	Local2 = PCRB (Local0)
	Local2 += PAD_CFG_BASE
	Return (Local2 + (Local1 * 8))
}

/*
 * Return PCR Port ID of GPIO Communities
 *
 * Arg0: GPIO Community (0-5)
 */
Method (GPID, 1, Serialized)
{
	Switch (ToInteger (Arg0))
	{
		Case (COMM_0)
		{
			Local0 = PID_GPIOCOM0
		}

		Case (COMM_1)
		{
			Local0 = PID_GPIOCOM1
		}

		Case (COMM_2)
		{
			Local0 = PID_GPIOCOM2
		}

		Case (COMM_3)
		{
			Local0 = PID_GPIOCOM3
		}

		Case (COMM_4)
		{
			Local0 = PID_GPIOCOM4
		}

		Case (COMM_5)
		{
			Local0 = PID_GPIOCOM5
		}

		Default
		{
			Return (0)
		}
	}
	Return (Local0)
}
