/* SPDX-License-Identifier: GPL-2.0-only */
#include <soc/gpio_defs_cnp_h.h>
#include <soc/intel/common/block/acpi/acpi/gpio_op.asl>
#include <soc/irq.h>
#include <soc/pcr_ids.h>

Device (GPIO)
{
	Name (_HID, "INT3450")
	Name (_UID, 0)
	Name (_DDN, "GPIO Controller")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0, COM0)
		Memory32Fixed (ReadWrite, 0, 0, COM1)
		Memory32Fixed (ReadWrite, 0, 0, COM3)
		Memory32Fixed (ReadWrite, 0, 0, COM4)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ)
			{ GPIO_IRQ14 }
	})

	/*
	 * GPIO communities 0, 1, 3, and 4 are exported for the OS.
	 * This is based on the Linux kernel provided community map at
	 * drivers/pinctrl/intel/pinctrl-cannonlake.c:cnhl_communities[]
	 */
	Method (_CRS, 0, NotSerialized)
	{
		/* GPIO Community 0 */
		CreateDWordField (^RBUF, ^COM0._BAS, BAS0)
		CreateDWordField (^RBUF, ^COM0._LEN, LEN0)
		Store (^^PCRB (PID_GPIOCOM0), BAS0)
		Store (GPIO_BASE_SIZE, LEN0)

		/* GPIO Community 1 */
		CreateDWordField (^RBUF, ^COM1._BAS, BAS1)
		CreateDWordField (^RBUF, ^COM1._LEN, LEN1)
		Store (^^PCRB (PID_GPIOCOM1), BAS1)
		Store (GPIO_BASE_SIZE, LEN1)

		/* GPIO Community 3 */
		CreateDWordField (^RBUF, ^COM3._BAS, BAS3)
		CreateDWordField (^RBUF, ^COM3._LEN, LEN3)
		Store (^^PCRB (PID_GPIOCOM3), BAS3)
		Store (GPIO_BASE_SIZE, LEN3)

		/* GPIO Community 4 */
		CreateDWordField (^RBUF, ^COM4._BAS, BAS4)
		CreateDWordField (^RBUF, ^COM4._LEN, LEN4)
		Store (^^PCRB (PID_GPIOCOM4), BAS4)
		Store (GPIO_BASE_SIZE, LEN4)

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
	If (Arg0 >= GPP_A0 && Arg0 <= GSPI1_CLK_LOOPBK)
	{
		Store (PID_GPIOCOM0, Local0)
		Local1 = Arg0 - GPP_A0
	}
	/* GPIO Community 1 */
	If (Arg0 >= GPP_C0 && Arg0 <= vSSP2_RXD)
	{
		Store (PID_GPIOCOM1, Local0)
		Local1 = Arg0 - GPP_C0
	}
	/* GPIO Community 3*/
	If (Arg0 >= GPP_K0 && Arg0 <= SPI0_CLK_LOOPBK)
	{
		Store (PID_GPIOCOM3, Local0)
		Local1 = Arg0 - GPP_K0
	}
	/* GPIO Community 4*/
	If (Arg0 >= HDACPU_SDI && Arg0 <= GPP_J11)
	{
		Store (PID_GPIOCOM4, Local0)
		Local1 = Arg0 - GPP_I0
	}
	Store (PCRB (Local0), Local2)
	Local2 += PAD_CFG_BASE
	Return (Local2 + Local1 * 16)
}
