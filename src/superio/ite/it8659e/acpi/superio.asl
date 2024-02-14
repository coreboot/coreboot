/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Include this file into a mainboard's DSDT _SB device tree and it will
 * expose the IT8659E SuperIO and some of its functionality.
 *
 * It allows the change of IO ports, IRQs and DMA settings on logical
 * devices, disabling and reenabling logical devices.
 *
 *   LDN		State
 * 0x1 UARTA		Implemented, tested
 * 0x2 UARTB		Implemented, tested
 * 0x4 EC		Implemented, tested
 * 0x5 KBC		Implemented, untested
 * 0x6 MOUSE		Implemented, untested
 * 0x7 GPIO		Implemented, tested
 * 0xa CIR		Not implemented
 *
 * Controllable through preprocessor defines:
 * SUPERIO_DEV		Device identifier for this SIO (e.g. SIO0)
 * SUPERIO_PNP_BASE	I/O address of the first PnP configuration register
 * IT8659E_SHOW_UARTA	If defined, UARTA will be exposed.
 * IT8659E_SHOW_UARTB	If defined, UARTB will be exposed.
 * IT8659E_SHOW_KBC	If defined, the KBC will be exposed.
 * IT8659E_SHOW_PS2M	If defined, PS/2 mouse support will be exposed.
 * IT8659E_SHOW_EC	If defined, the EC will be exposed.
 * IT8659E_SHOW_GPIO	If defined, the GPIO will be exposed.
 */

#undef SUPERIO_CHIP_NAME
#define SUPERIO_CHIP_NAME IT8659E
#include <superio/acpi/pnp.asl>

#undef PNP_DEFAULT_PSC
#define PNP_DEFAULT_PSC Return (0) /* no power management */

#define CONFIGURE_CONTROL CCTL

Device (SUPERIO_DEV) {
	Name (_HID, EisaId("PNP0A05"))
	Name (_STR, Unicode("ITE IT8659E Super I/O"))
	Name (_UID, SUPERIO_UID(SUPERIO_DEV,))

	/* Mutex for accesses to the configuration ports */
	Mutex (CRMX, 1)

	/* SuperIO configuration ports */
	OperationRegion (CREG, SystemIO, SUPERIO_PNP_BASE, 0x02)
	Field (CREG, ByteAcc, NoLock, Preserve)
	{
		PNP_ADDR_REG,	8,
		PNP_DATA_REG,	8
	}
	IndexField (PNP_ADDR_REG, PNP_DATA_REG, ByteAcc, NoLock, Preserve)
	{
		Offset (0x02),
		CONFIGURE_CONTROL,	8, /* Global configure control */

		Offset (0x07),
		PNP_LOGICAL_DEVICE,	8, /* Logical device selector */

		Offset (0x30),
		PNP_DEVICE_ACTIVE,	1, /* Logical device activation */

		Offset (0x60),
		PNP_IO0_HIGH_BYTE,	8, /* First I/O port base - high byte */
		PNP_IO0_LOW_BYTE,	8, /* First I/O port base - low byte */
		PNP_IO1_HIGH_BYTE,	8, /* Second I/O port base - high byte */
		PNP_IO1_LOW_BYTE,	8, /* Second I/O port base - low byte */

		Offset (0x70),
		PNP_IRQ0,		8, /* First IRQ */
	}

	Method (_CRS)
	{
		/* Announce the used i/o ports to the OS */
		Return (ResourceTemplate () {
			IO (Decode16, SUPERIO_PNP_BASE, SUPERIO_PNP_BASE, 0x01, 0x02)
		})
	}

	#undef PNP_ENTER_MAGIC_1ST
	#undef PNP_ENTER_MAGIC_2ND
	#undef PNP_ENTER_MAGIC_3RD
	#undef PNP_ENTER_MAGIC_4TH
	#undef PNP_EXIT_MAGIC_1ST
	#define PNP_ENTER_MAGIC_1ST	0x87
	#define PNP_ENTER_MAGIC_2ND	0x01
	#define PNP_ENTER_MAGIC_3RD	0x55
#if SUPERIO_PNP_BASE == 0x2e
	#define PNP_ENTER_MAGIC_4TH	0x55
#else
	#define PNP_ENTER_MAGIC_4TH	0xaa
#endif
	#define PNP_EXIT_SPECIAL_REG	CONFIGURE_CONTROL
	#define PNP_EXIT_SPECIAL_VAL	0x02
	#include <superio/acpi/pnp_config.asl>

#ifdef IT8659E_SHOW_UARTA
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_DDN
	#undef SUPERIO_PNP_NO_DIS
	#undef SUPERIO_UART_PM_REG
	#undef SUPERIO_UART_PM_VAL
	#undef SUPERIO_UART_PM_LDN
	#define SUPERIO_UART_LDN 1
	#include <superio/acpi/pnp_uart.asl>
#endif

#ifdef IT8659E_SHOW_UARTB
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_DDN
	#undef SUPERIO_PNP_NO_DIS
	#undef SUPERIO_UART_PM_REG
	#undef SUPERIO_UART_PM_VAL
	#undef SUPERIO_UART_PM_LDN
	#define SUPERIO_UART_LDN 2
	#include <superio/acpi/pnp_uart.asl>
#endif

#ifdef IT8659E_SHOW_KBC
	#undef SUPERIO_KBC_LDN
	#undef SUPERIO_KBC_PS2M
	#undef SUPERIO_KBC_PS2LDN
	#undef SUPERIO_PNP_NO_DIS
	#define SUPERIO_KBC_LDN 5
#ifdef IT8659E_SHOW_PS2M
	#define SUPERIO_KBC_PS2LDN 6
#endif
	#include <superio/acpi/pnp_kbc.asl>
#endif


/*
 * Generic setup for EC device.
 *
 * IT8659E_EC_IO0	The alignment and length of the first PnP i/o
 *			resource (comma separated, e.g. `0x02, 0x08`,
 *			optional)
 * IT8659E_EC_IO1	The alignment and length of the second PnP i/o
 *			resource (comma separated, e.g. `0x02, 0x08`,
 *			optional)
 * IT8659E_EC_IRQ0	If defined, the first PnP IRQ register is enabled
 */
#ifdef IT8659E_SHOW_EC
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

	#define SUPERIO_PNP_LDN 4
	#define SUPERIO_PNP_DDN "ITE IT8659E Environmental Controller"

#ifdef IT8659E_EC_IO0
	#define SUPERIO_PNP_IO0 0x08, 0x08
#endif

#ifdef IT8659E_EC_IO1
	#define SUPERIO_PNP_IO1 0x08, 0x04
#endif

#ifdef IT8659E_EC_IRQ0
	#define SUPERIO_PNP_IRQ0
#endif

	#include <superio/acpi/pnp_generic.asl>
#endif

/*
 * Generic setup for GPIO device.
 *
 * IT8659E_EC_IO0	The alignment and length of the first PnP i/o
 *			resource (comma separated, e.g. `0x02, 0x08`,
 *			optional)
 * IT8659E_EC_IO1	The alignment and length of the second PnP i/o
 *			resource (comma separated, e.g. `0x02, 0x08`,
 *			optional)
 * IT8659E_EC_IRQ0	If defined, the first PnP IRQ register is enabled
 */
#ifdef IT8659E_SHOW_GPIO
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

	#define SUPERIO_PNP_LDN 7
	#define SUPERIO_PNP_DDN "ITE IT8659E GPIO"
	#define SUPERIO_PNP_NO_DIS

#ifdef IT8659E_GPIO_IO0
	#define SUPERIO_PNP_IO0 0x04, 0x04
#endif

#ifdef IT8659E_GPIO_IO1
	#define SUPERIO_PNP_IO1 0x01, 0x08
#endif
	#include <superio/acpi/pnp_generic.asl>
#endif

}
