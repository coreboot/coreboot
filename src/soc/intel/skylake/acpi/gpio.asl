/* SPDX-License-Identifier: GPL-2.0-only */
#include <gpio.h>
#include <soc/intel/common/block/acpi/acpi/gpio_op.asl>

Device (GPIO)
{
	Name (_HID, "INT344B")
	Name (_UID, 1)
	Name (_DDN, "GPIO Controller")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0, COM0)
		Memory32Fixed (ReadWrite, 0, 0, COM1)
		Memory32Fixed (ReadWrite, 0, 0, COM3)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ)
			{ 0 }
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

		/* GPIO Community 3 */
		CreateDWordField (^RBUF, ^COM3._BAS, BAS3)
		CreateDWordField (^RBUF, ^COM3._LEN, LEN3)
		BAS3 = ^^PCRB (PID_GPIOCOM3)
		LEN3 = GPIO_BASE_SIZE

		CreateDWordField (^RBUF, ^GIRQ._INT, IRQN)
		Local0 = ^^PCRR (PID_GPIOCOM0, GPIO_MISCCFG) & GPIO_DRIVER_IRQ_ROUTE_MASK

		If (Local0 == GPIO_DRIVER_IRQ_ROUTE_IRQ14) {
			IRQN = GPIO_IRQ14
		} Else {
			IRQN = GPIO_IRQ15
		}

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
#if CONFIG(SKYLAKE_SOC_PCH_H)
	/* GPIO Community 0 */
	If ((Arg0 >= GPP_A0) && (Arg0 <= GPP_B23))
	{
		Local0 = PID_GPIOCOM0
		Local1 = Arg0 - GPP_A0
	}
	/* GPIO Community 1 */
	If ((Arg0 >= GPP_C0) && (Arg0 <= GPP_H23))
	{
		Local0 = PID_GPIOCOM1
		Local1 = Arg0 - GPP_C0
	}
	/* GPIO Community 3 */
	If ((Arg0 >= GPP_I0) && (Arg0 <= GPP_I10))
	{
		Local0 = PID_GPIOCOM3
		Local1 = Arg0 - GPP_I0
	}
#else
	/* GPIO Community 0 */
	If ((Arg0 >= GPP_A0) && (Arg0 <= GPP_B23))
	{
		Local0 = PID_GPIOCOM0
		Local1 = Arg0 - GPP_A0
	}
	/* GPIO Community 1 */
	If ((Arg0 >= GPP_C0) && (Arg0 <= GPP_E23))
	{
		Local0 = PID_GPIOCOM1
		Local1 = Arg0 - GPP_C0
	}
	/* GPIO Community 3 */
	If ((Arg0 >= GPP_F0) && (Arg0 <= GPP_G7))
	{
		Local0 = PID_GPIOCOM3
		Local1 = Arg0 - GPP_F0
	}
#endif /* CONFIG_SKYLAKE_SOC_PCH_H */

	Local2 = PCRB (Local0)
	Local2 += PAD_CFG_BASE
	Return (Local2 + (Local1 * 8))
}
