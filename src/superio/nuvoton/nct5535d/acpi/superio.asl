/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Include this file into a mainboard's DSDT _SB device tree and it will
 * expose the NCT5535D SuperIO and some of its functionality.
 *
 * It allows the change of IO ports, IRQs and DMA settings on logical
 * devices, disabling and reenabling logical devices.
 *
 *   LDN		State
 * 0x2 SP1		Implemented, untested
 * 0x5 KBC		Implemented, untested
 * 0x8 GPIO		Implemented, untested
 * 0xb HWM		Implemented, untested
 *
 * Controllable through preprocessor defines:
 * SUPERIO_DEV		Device identifier for this SIO (e.g. SIO0)
 * SUPERIO_PNP_BASE	I/O address of the first PnP configuration register
 * NCT5535D_SHOW_SP1	If defined, Serial Port 1 will be exposed.
 * NCT5535D_SHOW_KBC	If defined, the Keyboard Controller will be exposed.
 * NCT5535D_SHOW_GPIO	If defined, GPIO support will be exposed.
 * NCT5535D_SHOW_HWM	If defined, the Environment Controller will be exposed.
 */
#include <superio/nuvoton/nct5535d/nct5535d.h>

#undef SUPERIO_CHIP_NAME
#define SUPERIO_CHIP_NAME NCT5535D
#include <superio/acpi/pnp.asl>

#undef PNP_DEFAULT_PSC
#define PNP_DEFAULT_PSC Return (0) /* no power management */

#define ENABLE_KEYBOARD_WAKEUP SKWK
#define POWER_LOSS_CONTROL     SPWL

Device(SUPERIO_DEV) {
	Name (_HID, EisaId("PNP0A05"))
	Name (_STR, Unicode("Nuvoton NCT5535D Super I/O"))
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

		Offset (0x70),
		PNP_IRQ0,		8, /* First IRQ */
		Offset (0x72),
		PNP_IRQ1,		8, /* Second IRQ */
		Offset (0x74),
		PNP_DMA0,		8, /* DRQ */
		Offset (0xe0),		   /* Config register 0xe0 etc. */
		,6,
		ENABLE_KEYBOARD_WAKEUP,	1,
		Offset (0xe4),
		,5,
		POWER_LOSS_CONTROL,	2,
	}

	Method (_CRS)
	{
		/* Announce the used I/O ports to the OS */
		Return (ResourceTemplate () {
			IO (Decode16, SUPERIO_PNP_BASE, SUPERIO_PNP_BASE, 0x01, 0x02)
		})
	}

	Method (SIOS, 1, NotSerialized)
	{
		if (Arg0 == 5)
		{
			ENTER_CONFIG_MODE(NCT5535D_ACPI)
			ENABLE_KEYBOARD_WAKEUP = 0
			EXIT_CONFIG_MODE()
		}
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

#ifdef NCT5535D_SHOW_SP1
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_DDN
	#undef SUPERIO_UART_PM_REG
	#undef SUPERIO_UART_PM_VAL
	#undef SUPERIO_UART_PM_LDN
	#define SUPERIO_UART_LDN NCT5535D_SP1
	#include <superio/acpi/pnp_uart.asl>
#endif

#ifdef NCT5535D_SHOW_KBC
	#undef SUPERIO_KBC_LDN
	#undef SUPERIO_KBC_PS2M
	#undef SUPERIO_KBC_PS2LDN
	#define SUPERIO_KBC_LDN NCT5535D_KBC
	#define SUPERIO_KBC_PS2M
	#include <superio/acpi/pnp_kbc.asl>
#endif

#ifdef NCT5535D_SHOW_HWM
	#undef SUPERIO_PNP_HID
	#undef SUPERIO_PNP_LDN
	#undef SUPERIO_PNP_DDN
	#undef SUPERIO_PNP_NO_DIS
	#undef SUPERIO_PNP_PM_REG
	#undef SUPERIO_PNP_PM_VAL
	#undef SUPERIO_PNP_PM_LDN
	#undef SUPERIO_PNP_IO0
	#undef SUPERIO_PNP_IO1
	#undef SUPERIO_PNP_IO2
	#undef SUPERIO_PNP_IRQ0
	#undef SUPERIO_PNP_IRQ1
	#undef SUPERIO_PNP_DMA
	#define SUPERIO_PNP_LDN NCT5535D_HWM_FPLED
	#define SUPERIO_PNP_IO0 0x08, 0x08
	#define SUPERIO_PNP_IO1 0x08, 0x08
	#define SUPERIO_PNP_IRQ0
	#include <superio/acpi/pnp_generic.asl>
#endif

#ifdef NCT5535D_SHOW_GPIO
	#undef SUPERIO_PNP_HID
	#undef SUPERIO_PNP_LDN
	#undef SUPERIO_PNP_DDN
	#undef SUPERIO_PNP_NO_DIS
	#undef SUPERIO_PNP_PM_REG
	#undef SUPERIO_PNP_PM_VAL
	#undef SUPERIO_PNP_PM_LDN
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
