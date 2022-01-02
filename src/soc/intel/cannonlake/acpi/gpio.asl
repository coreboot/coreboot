/* SPDX-License-Identifier: GPL-2.0-only */
#include <soc/gpio_defs.h>
#include <soc/intel/common/block/acpi/acpi/gpio_op.asl>
#include <soc/irq.h>
#include <soc/pcr_ids.h>
#include <intelblocks/gpio.h>

Device (GPIO)
{
	Name (_HID, "INT34BB")
	Name (_UID, 0)
	Name (_DDN, "GPIO Controller")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0, COM0)
		Memory32Fixed (ReadWrite, 0, 0, COM1)
		Memory32Fixed (ReadWrite, 0, 0, COM4)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ)
			{ GPIO_IRQ14 }
	})

	/*
	 * GPIO communities 0, 1, and 4 are exported for the OS.
	 * This is based on the Linux kernel provided community map at
	 * drivers/pinctrl/intel/pinctrl-cannonlake.c:cnllp_communities[]
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
	If (Arg0 >= GPP_A0 && LLessEqual (Arg0, SPI0_CLK_LOOPBK))
	{
		Store (PID_GPIOCOM0, Local0)
		Local1 = Arg0 - GPP_A0
	}
	/* GPIO Community 1 */
	If (Arg0 >= GPP_D0 && LLessEqual (Arg0, vSD3_CD_B))
	{
		Store (PID_GPIOCOM1, Local0)
		Local1 = Arg0 - GPP_D0
	}
	/* GPIO Community 2 */
	If (Arg0 >= GPD0 && LLessEqual (Arg0, DRAM_RESET_B))
	{
		Store (PID_GPIOCOM2, Local0)
		Local1 = Arg0 - GPD0
	}
	/* GPIO Community 3 */
	If (Arg0 >= HDA_BCLK && LLessEqual (Arg0, TRIGGER_OUT))
	{
		Store (PID_GPIOCOM3, Local0)
		Local1 = Arg0 - HDA_BCLK
	}
	/* GPIO Community 4*/
	If (Arg0 >= GPP_C0 && LLessEqual (Arg0, CL_RST_B))
	{
		Store (PID_GPIOCOM4, Local0)
		Local1 = Arg0 - GPP_C0
	}
	Store (PCRB (Local0), Local2)
	Local2 += PAD_CFG_BASE
	Return (Local2 + Local1 * 16)
}

/*
 * Return PCR Port ID of GPIO Communities
 *
 * Arg0: GPIO Community (0-4)
 */
Method (GPID, 1, Serialized)
{
	Switch (ToInteger (Arg0))
	{
		Case (0) {
			Store (PID_GPIOCOM0, Local0)
		}
		Case (1) {
			Store (PID_GPIOCOM1, Local0)
		}
		Case (2) {
			Store (PID_GPIOCOM2, Local0)
		}
		Case (3) {
			Store (PID_GPIOCOM3, Local0)
		}
		Case (4) {
			Store (PID_GPIOCOM4, Local0)
		}
		Default {
			Return (0)
		}
	}

	Return (Local0)
}

/*
 * Configure GPIO Power Management bits
 *
 * Arg0: GPIO community (0-4)
 * Arg1: PM bits in MISCCFG
 */
Method (CGPM, 2, Serialized)
{
	Store (GPID (Arg0), Local0)
	If (LNotEqual (Local0, 0)) {
		/* Mask off current PM bits */
		PCRA (Local0, GPIO_MISCCFG, Not (MISCCFG_GPIO_PM_CONFIG_BITS))
		/* Mask in requested bits */
		PCRO (Local0, GPIO_MISCCFG, And (Arg1, MISCCFG_GPIO_PM_CONFIG_BITS))
	}
}

/* GPIO Power Management bits */
Name(GPMB, Package(TOTAL_GPIO_COMM) {0, 0, 0, 0, 0})

/*
 * Save GPIO Power Management bits
 */
Method (SGPM, 0, Serialized)
{
	For (Local0 = 0, Local0 < TOTAL_GPIO_COMM, Local0++)
	{
		Local1 = GPID (Local0)
		GPMB[Local0] = PCRR (Local1, GPIO_MISCCFG)
	}
}

/*
 * Restore GPIO Power Management bits
 */
Method (RGPM, 0, Serialized)
{
	For (Local0 = 0, Local0 < TOTAL_GPIO_COMM, Local0++)
	{
		CGPM (Local0, DerefOf(GPMB[Local0]))
	}
}

/*
 * Save current setting of GPIO Power Management bits and
 * enable all Power Management bits for all communities
 */
Method (EGPM, 0, Serialized)
{
	/* Save current setting and will restore it when resuming */
	SGPM ()
	/* Enable PM bits */
	For (Local0 = 0, Local0 < TOTAL_GPIO_COMM, Local0++)
	{
		CGPM (Local0, MISCCFG_GPIO_PM_CONFIG_BITS)
	}
}
