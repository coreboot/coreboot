/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Include this file into a mainboard's DSDT _SB device tree and it will
 * expose the W83627DHG SuperIO and some of its functionality.
 *
 * It allows the change of IO ports, IRQs and DMA settings on logical
 * devices, disabling and reenabling logical devices and controlling power
 * saving mode on logical devices or the whole chip.
 *
 *   LDN		State
 * 0x0 FDC		Not implemented
 * 0x1 PP		Not implemented
 * 0x2 UARTA		Implemented, partially tested
 * 0x3 UARTB		UART only, partially tested
 * 0x5 KBC		Implemented, untested
 * 0x6 SPI		Not implemented
 * 0x7 GPIO6		Not implemented
 * 0x8 WDT0&PLED	Not implemented
 * 0x9 GPIO2-5		Not implemented
 * 0xa ACPI		Not implemented
 * 0xb HWM		Resources, PM only
 * 0xc PECI&SST		Not implemented
 *
 * Controllable through preprocessor defines:
 * SUPERIO_DEV		Device identifier for this SIO (e.g. SIO0)
 * SUPERIO_PNP_BASE	I/o address of the first PnP configuration register
 * W83627DHG_SHOW_UARTA	If defined, UARTA will be exposed.
 * W83627DHG_SHOW_UARTB	If defined, UARTB will be exposed.
 * W83627DHG_SHOW_KBC	If defined, the KBC will be exposed.
 * W83627DHG_SHOW_PS2M	If defined, PS/2 mouse support will be exposed.
 * W83627DHG_SHOW_HWMON	If defined, the hardware monitor will be exposed.
 */

#undef SUPERIO_CHIP_NAME
#define SUPERIO_CHIP_NAME W83627DHG
#include <superio/acpi/pnp.asl>

Device(SUPERIO_DEV) {
	Name (_HID, EisaId("PNP0A05"))
	Name (_STR, Unicode("Winbond W83627DHG Super I/O"))
	Name (_UID, SUPERIO_UID(SUPERIO_DEV,))

	/* Mutex for accesses to the configuration ports */
	Mutex(CRMX, 1)

	/* SuperIO configuration ports */
	OperationRegion (CREG, SystemIO, SUPERIO_PNP_BASE, 0x02)
	Field (CREG, ByteAcc, NoLock, Preserve)
	{
		PNP_ADDR_REG,	8,
		PNP_DATA_REG,	8
	}
	IndexField (PNP_ADDR_REG, PNP_DATA_REG, ByteAcc, NoLock, Preserve)
	{
		Offset (0x07),
		PNP_LOGICAL_DEVICE,	8, /* Logical device selector */

		Offset (0x22),
		FDPW,			1, /* FDC Power Down */
		,			2,
		PRPW,			1, /* PRT Power Down */
		UAPW,			1, /* UART A Power Down */
		UBPW,			1, /* UART B Power Down */
		HWPW,			1, /* HWM Power Down */
		Offset (0x23),
		IPD,			1, /* Immediate Chip Power Down */

		Offset (0x30),
		PNP_DEVICE_ACTIVE,	1, /* Logical device activation */

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
		PNP_DMA0,		8, /* DMA */

		Offset (0xf3),
		SULM,			8, /* Suspend LED Mode Register */
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
	#undef PNP_EXIT_SPECIAL_REG
	#undef PNP_EXIT_SPECIAL_VAL
	#define PNP_ENTER_MAGIC_1ST	0x87
	#define PNP_ENTER_MAGIC_2ND	0x87
	#define PNP_EXIT_MAGIC_1ST	0xaa
	#include <superio/acpi/pnp_config.asl>

	/* PM: indicate IPD (Immediate Power Down) bit state as D0/D3 */
	Method (_PSC) {
		ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
		  Local0 = IPD
		EXIT_CONFIG_MODE ()
		If (Local0) { Return (3) }
		Else { Return (0) }
	}

	/* PM: Switch to D0 by setting IPD low */
	Method (_PS0) {
		ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
		  IPD = 0
		EXIT_CONFIG_MODE ()
	}

	/* PM: Switch to D3 by setting IPD high */
	Method (_PS3) {
		ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
		  IPD = 1
		EXIT_CONFIG_MODE ()
	}

	/* Suspend LED: Write given three-bit value into appropriate register.
			From the datasheet:
			000 - drive pin constantly high
			001 - drive 0.5Hz pulses
			010 - drive pin constantly low
			011 - drive 2Hz pulses
			100 - drive 1Hz pulses
			101 - drive 4Hz pulses
			110 - drive 0.25Hz pulses
			111 - drive 0.25Hz pulses
			(all pulses with 50% duty cycle) */
	#define SUPERIO_SUSL_LDN 9
	Method (SUSL, 1, Serialized) {
		ENTER_CONFIG_MODE (SUPERIO_SUSL_LDN)
		  Local0 = SULM
		  Local0 &= 0x1f
		  Local0 |= (Arg0 << 5)
		  SULM = Local0
		EXIT_CONFIG_MODE ()
	}

#ifdef W83627DHG_SHOW_UARTA
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_DDN
	#undef SUPERIO_UART_PM_REG
	#undef SUPERIO_UART_PM_VAL
	#undef SUPERIO_UART_PM_LDN
	#define SUPERIO_UART_LDN 2
	#define SUPERIO_UART_PM_REG UAPW
	#define SUPERIO_UART_PM_VAL 0
	#define SUPERIO_UART_PM_LDN PNP_NO_LDN_CHANGE
	#include <superio/acpi/pnp_uart.asl>
#endif

#ifdef W83627DHG_SHOW_UARTB
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_DDN
	#undef SUPERIO_UART_PM_REG
	#undef SUPERIO_UART_PM_VAL
	#undef SUPERIO_UART_PM_LDN
	#define SUPERIO_UART_LDN 3
	#define SUPERIO_UART_PM_REG UBPW
	#define SUPERIO_UART_PM_VAL 0
	#define SUPERIO_UART_PM_LDN PNP_NO_LDN_CHANGE
	#include <superio/acpi/pnp_uart.asl>
#endif

#ifdef W83627DHG_SHOW_KBC
	#undef SUPERIO_KBC_LDN
	#undef SUPERIO_KBC_PS2M
	#undef SUPERIO_KBC_PS2LDN
	#define SUPERIO_KBC_LDN 5
#ifdef W83627DHG_SHOW_PS2M
	#define SUPERIO_KBC_PS2M 1
#endif
	#include <superio/acpi/pnp_kbc.asl>
#endif

#ifdef W83627DHG_SHOW_HWMON
	#undef SUPERIO_PNP_LDN
	#undef SUPERIO_PNP_DDN
	#undef SUPERIO_PNP_PM_REG
	#undef SUPERIO_PNP_PM_VAL
	#undef SUPERIO_PNP_PM_LDN
	#undef SUPERIO_PNP_IO0
	#undef SUPERIO_PNP_IO1
	#undef SUPERIO_PNP_IRQ0
	#undef SUPERIO_PNP_IRQ1
	#undef SUPERIO_PNP_DMA
	#define SUPERIO_PNP_LDN		11
	#define SUPERIO_PNP_PM_REG	HWPW
	#define SUPERIO_PNP_PM_VAL	0
	#define SUPERIO_PNP_PM_LDN	PNP_NO_LDN_CHANGE
	#define SUPERIO_PNP_IO0		0x08, 0x08
	#define SUPERIO_PNP_IRQ0	1
	#include <superio/acpi/pnp_generic.asl>
#endif
}
