/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <soc/intel/common/acpi/gpio.asl>
#include <soc/intel/common/block/acpi/acpi/gpio_op.asl>
#include <soc/irq.h>
#include <soc/pcr_ids.h>

Device (GPIO)
{
	Name (_HID, CROS_GPIO_NAME)
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
	/* GPIO Community 3 */
	If (Arg0 >= GPIO_COM3_START && Arg0 <= GPIO_COM3_END)
	{
		Local0 = PID_GPIOCOM3
		Local1 = Arg0 - GPIO_COM3_START
	}
	/* GPIO Community 4 */
	If (Arg0 >= GPIO_COM4_START && Arg0 <= GPIO_COM4_END)
	{
		Local0 = PID_GPIOCOM4
		Local1 = Arg0 - GPIO_COM4_START
	}
	/* GPIO Community 5 */
	If (Arg0 >= GPIO_COM5_START && Arg0 <= GPIO_COM5_END)
	{
		Local0 = PID_GPIOCOM5
		Local1 = Arg0 - GPIO_COM5_START
	}

	Local2 = PCRB(Local0) + PAD_CFG_BASE + (Local1 * 16)
	Return (Local2)
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
		Case (COMM_0) {
			Local0 = PID_GPIOCOM0
		}
		Case (COMM_1) {
			Local0 = PID_GPIOCOM1
		}
		Case (COMM_2) {
			Local0 = PID_GPIOCOM2
		}
		Case (COMM_3) {
			Local0 = PID_GPIOCOM3
		}
		Case (COMM_4) {
			Local0 = PID_GPIOCOM4
		}
		Case (COMM_5) {
			Local0 = PID_GPIOCOM5
		}
		Default {
			Return (0)
		}
	}

	Return (Local0)
}
