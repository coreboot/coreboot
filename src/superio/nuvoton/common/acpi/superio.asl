/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Include this file into a mainboard's DSDT _SB device tree and it will
 * expose the NCT677x SuperIO and some of its functionality.
 *
 * It allows the change of IO ports, IRQs and DMA settings on logical
 * devices, disabling and reenabling logical devices.
 *
 *   LDN		State
 * NCT677X_PP		Implemented, untested
 * NCT677x_SP1		Implemented, untested
 * NCT677x_SP2		Implemented, untested
 * NCT677x_KBC		Implemented, untested
 * 0x308 GPIO		Implemented, untested
 * 0xb HWM		Implemented, untested
 *
 * Controllable through preprocessor defines:
 *
 * SIO identities
 * SUPERIO_DEV		Device identifier for this SIO (e.g. SIO0; required)
 * SUPERIO_CHIP_NAME	Chip name (@)
 * SUPERIO_FULL_CHIP_NAME	Unicode device name (@)
 * SUPERIO_PNP_BASE	I/O address of the first PnP configuration register (@)
 * Items with @ are required if multiple SIO chips are being used.
 *
 * Functionality exposed if defined:
 * NCT677X_SHOW_PP	Parallel port
 * NCT677X_SHOW_SP1	Serial port 1
 * NCT677X_SHOW_SP2	Serial port 2
 * NCT677X_SHOW_KBC	Keyboard controller
 * NCT677X_SHOW_GPIO	GPIO by I/O support
 * NCT677X_SHOW_HWM	Hardware monitor
 */
#include <superio/nuvoton/common/nuvoton.h>

/* Get SUPERIO_PNP_BASE from Kconfig if not redefined */
#ifndef SUPERIO_PNP_BASE
#define SUPERIO_PNP_BASE CONFIG_SUPERIO_NUVOTON_PNP_BASE
#endif

#include <superio/acpi/pnp.asl>

#undef PNP_DEFAULT_PSC
#define PNP_DEFAULT_PSC Return (0) /* no power management */

#define ENABLE_KEYBOARD_WAKEUP SKWK
#define POWER_LOSS_LAST_STATE  PLSF

Device(SUPERIO_DEV) {
	Name (_HID, EisaId("PNP0A05"))
	Name (_STR, Unicode(SUPERIO_FULL_CHIP_NAME))
	Name (_UID, SUPERIO_UID(SUPERIO_DEV,))

	/* SuperIO configuration ports */
	OperationRegion (CREG, SystemIO, SUPERIO_PNP_BASE, 2)
	Field (CREG, ByteAcc, NoLock, Preserve)
	{
		PNP_ADDR_REG,	8,
		PNP_DATA_REG,	8,
	}
	IndexField (PNP_ADDR_REG, PNP_DATA_REG, ByteAcc, NoLock, Preserve)
	{
		Offset (0x07),
		PNP_LOGICAL_DEVICE,	8, /* Logical device selector */

		Offset (0x30),
		PNP_DEVICE_ACTIVE,	1, /* Logical device activation */
		ACT1,			1, /* Logical device activation */
		ACT2,			1, /* Logical device activation */
		ACT3,			1, /* Logical device activation */
		ACT4,			1, /* Logical device activation */
		ACT5,			1, /* Logical device activation */
		ACT6,			1, /* Logical device activation */
		ACT7,			1, /* Logical device activation */

		Offset (0x60),
		PNP_IO0_HIGH_BYTE,	8, /* First I/O port base - high byte */
		PNP_IO0_LOW_BYTE,	8, /* First I/O port base - low byte */
		Offset (0x62),
		PNP_IO1_HIGH_BYTE,	8, /* Second I/O port base - high byte */
		PNP_IO1_LOW_BYTE,	8, /* Second I/O port base - low byte */
		Offset (0x64),
		PNP_IO2_HIGH_BYTE,	8, /* Third I/O port base - high byte */
		PNP_IO2_LOW_BYTE,	8, /* Third I/O port base - low byte */
		Offset (0x70),
		PNP_IRQ0,		8, /* First IRQ */
		Offset (0x72),
		PNP_IRQ1,		8, /* Second IRQ */
		Offset (0x74),
		PNP_DMA0,		8, /* DRQ */

		Offset (0xe0),		   /* Config register 0xe0 etc. */
		,6,
		ENABLE_KEYBOARD_WAKEUP,	1,
		Offset (0xe6),
		,4,
		POWER_LOSS_LAST_STATE,	1,
		Offset (0xf2),
		SPME,			1, /* Enable PME */
		,7,
		OPT3,			8,
		OPT4,			8,
		,8,
		OPT6,			8,
		OPT7,			8
	}

	Method (_CRS)
	{
		/* Announce the used I/O ports to the OS */
		Return (ResourceTemplate () {
			IO (Decode16, SUPERIO_PNP_BASE, SUPERIO_PNP_BASE, 1, 2)
		})
	}

	Method (SIOS, 1, NotSerialized)
	{
		ENTER_CONFIG_MODE(NCT677X_ACPI)
		if (Arg0 == 5)
		{
			ENABLE_KEYBOARD_WAKEUP = 0
			/* Log "power off" state */
			POWER_LOSS_LAST_STATE = 1
		}
		EXIT_CONFIG_MODE()
	}

	Method (SIOW, 1, NotSerialized)
	{
	}

	#undef PNP_ENTER_MAGIC_1ST
	#undef PNP_ENTER_MAGIC_2ND
	#undef PNP_ENTER_MAGIC_3RD
	#undef PNP_ENTER_MAGIC_4TH
	#undef PNP_EXIT_MAGIC_1ST
	#undef PNP_EXIT_SPECIAL_REG
	#undef PNP_EXIT_SPECIAL_VAL
	#define PNP_ENTER_MAGIC_1ST	0x87
	#define PNP_ENTER_MAGIC_2ND	0x87
	#define PNP_EXIT_MAGIC_1ST	0xaa
	#include <superio/acpi/pnp_config.asl>

#ifdef NCT677X_SHOW_PP
	#undef SUPERIO_PNP_HID
	#undef SUPERIO_PNP_LDN
	#undef SUPERIO_PNP_IO0
	#undef SUPERIO_PNP_IRQ0
	#undef SUPERIO_PNP_DMA
	/*
	 * The extra code required to dynamically reflect ECP in the HID
	 * isn't currently justified, so the HID is hardcoded as not
	 * using ECP. "PNP0401" would indicate ECP.
	 */
	#define SUPERIO_PNP_HID "PNP0400"
	#define SUPERIO_PNP_LDN NCT677X_PP
	#define SUPERIO_PNP_IO0 0x08, 0x08
	#define SUPERIO_PNP_IRQ0
	#define SUPERIO_PNP_DMA
	#include <superio/acpi/pnp_generic.asl>
#endif

#undef SUPERIO_UART_DDN
#undef SUPERIO_UART_PM_REG

#ifdef NCT677X_SHOW_SP1
	#undef SUPERIO_UART_LDN
	#define SUPERIO_UART_LDN NCT677X_SP1
	#include <superio/acpi/pnp_uart.asl>
#endif

#ifdef NCT677X_SHOW_SP2
	#undef SUPERIO_UART_LDN
	#define SUPERIO_UART_LDN NCT677X_SP2
	#include <superio/acpi/pnp_uart.asl>
#endif

#ifdef NCT677X_SHOW_KBC
	#undef SUPERIO_KBC_LDN
	#undef SUPERIO_KBC_PS2M
	#undef SUPERIO_KBC_PS2LDN
	#define SUPERIO_KBC_LDN NCT677X_KBC
	#define SUPERIO_KBC_PS2M
	#include <superio/acpi/pnp_kbc.asl>
#endif

#undef SUPERIO_PNP_HID
#undef SUPERIO_PNP_DMA

#ifdef NCT677X_SHOW_HWM
	#undef SUPERIO_PNP_LDN
	#undef SUPERIO_PNP_IO0
	#undef SUPERIO_PNP_IO1
	#undef SUPERIO_PNP_IRQ0
	#define SUPERIO_PNP_LDN NCT677X_HWM_FPLED
	#define SUPERIO_PNP_IO0 0x08, 0x08
	#define SUPERIO_PNP_IO1 0x08, 0x08
	#define SUPERIO_PNP_IRQ0
	#include <superio/acpi/pnp_generic.asl>
#endif

#ifdef NCT677X_SHOW_GPIO
	#undef SUPERIO_PNP_LDN
	#undef SUPERIO_PNP_IO0
	#undef SUPERIO_PNP_IO1
	#undef SUPERIO_PNP_IRQ0
	#undef PNP_DEVICE_ACTIVE
	#define PNP_DEVICE_ACTIVE ACT3
	#define SUPERIO_PNP_LDN 8
	#define SUPERIO_PNP_IO0 0x08, 0x08
	#include <superio/acpi/pnp_generic.asl>
#endif
}
