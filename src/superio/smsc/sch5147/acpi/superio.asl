/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Include this file into a mainboard's DSDT _SB device tree and it will
 * expose the SCH5147 SuperIO and some of its functionality.
 *
 * It allows the change of IO ports, IRQs and DMA settings on logical
 * devices, disabling and reenabling logical devices and controlling power
 * saving mode on logical devices or the whole chip.
 *
 *   LDN		State
 * 0x0 FDC		Not implemented
 * 0x3 PP		Not implemented
 * 0x4 UARTA		Implemented
 * 0x5 UARTB		Implemented
 * 0x7 KBC		Implemented
 * 0xa Runtime reg	Not implemented
 *
 * Controllable through preprocessor defines:
 * SUPERIO_DEV		Device identifier for this SIO (e.g. SIO0)
 * SUPERIO_PNP_BASE	I/o address of the first PnP configuration register
 * SCH5147_SHOW_UARTA	If defined, UARTA will be exposed.
 * SCH5147_SHOW_UARTB	If defined, UARTB will be exposed.
 * SCH5147_SHOW_KBC	If defined, the KBC will be exposed.
 */

#undef SUPERIO_CHIP_NAME
#define SUPERIO_CHIP_NAME SCH5147
#include <superio/acpi/pnp.asl>

#undef PNP_DEFAULT_PSC
#define PNP_DEFAULT_PSC Return (0) /* no power management */

Device(SUPERIO_DEV) {
	Name (_HID, EisaId("PNP0A05"))
	Name (_STR, Unicode("SMSC SCH5147 Super I/O"))
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
	}

	Method (_CRS)
	{
		/* Announce the used i/o ports to the OS */
		Return (ResourceTemplate () {
			FixedIO (SUPERIO_PNP_BASE, 0x02)
		})
	}

	#undef PNP_ENTER_MAGIC_1ST
	#undef PNP_ENTER_MAGIC_2ND
	#undef PNP_ENTER_MAGIC_3RD
	#undef PNP_ENTER_MAGIC_4TH
	#undef PNP_EXIT_MAGIC_1ST
	#undef PNP_EXIT_SPECIAL_REG
	#undef PNP_EXIT_SPECIAL_VAL
	#define PNP_ENTER_MAGIC_1ST	0x55
	#define PNP_EXIT_MAGIC_1ST	0xaa
	#include <superio/acpi/pnp_config.asl>

#ifdef SCH5147_SHOW_UARTA
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_DDN
	#undef SUPERIO_UART_PM_REG
	#undef SUPERIO_UART_PM_VAL
	#undef SUPERIO_UART_PM_LDN
	#define SUPERIO_UART_LDN 4
	#include <superio/acpi/pnp_uart.asl>
#endif

#ifdef SCH5147_SHOW_UARTB
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_DDN
	#undef SUPERIO_UART_PM_REG
	#undef SUPERIO_UART_PM_VAL
	#undef SUPERIO_UART_PM_LDN
	#define SUPERIO_UART_LDN 5
	#include <superio/acpi/pnp_uart.asl>
#endif

#ifdef SCH5147_SHOW_KBC
	/* we can't read back the IO resources so hardcode them */
	#define SUPERIO_KBC_LDN 7
Device (SUPERIO_ID(KBD, SUPERIO_KBC_LDN)) {
	Name (_HID, EisaId ("PNP0303"))
	Name (_UID, SUPERIO_UID(KBD, SUPERIO_KBC_LDN))

	Method (_STA)
	{
		PNP_GENERIC_STA(SUPERIO_KBC_LDN)
	}

	Method (_DIS)
	{
		ENTER_CONFIG_MODE (SUPERIO_KBC_LDN)
		  PNP_DEVICE_ACTIVE = 0
		EXIT_CONFIG_MODE ()
		#if defined(SUPERIO_KBC_PS2LDN)
		Notify (SUPERIO_ID(PS2, SUPERIO_KBC_PS2LDN), 1)
		#elif defined(SUPERIO_KBC_PS2M)
		Notify (SUPERIO_ID(PS2, SUPERIO_KBC_LDN), 1)
		#endif
	}

	Method (_PSC) {
		PNP_DEFAULT_PSC
	}

	Method (_CRS, 0, Serialized)
	{
		Name (CRS, ResourceTemplate () {
			FixedIO (0x0060, 0x01)
			FixedIO (0x0064, 0x01)
			IRQNoFlags (IR0) {}
		})
		ENTER_CONFIG_MODE (SUPERIO_KBC_LDN)
		  PNP_READ_IRQ(PNP_IRQ0, CRS, IR0)
		EXIT_CONFIG_MODE ()
		Return (CRS)
	}

	Name (_PRS, ResourceTemplate ()
	{
		StartDependentFn (0,0) {
			FixedIO (0x0060, 0x01)
			FixedIO (0x0064, 0x01)
			IRQNoFlags () {1}
		}
		EndDependentFn()
	})

	Method (_SRS, 1, Serialized)
	{
		Name (TMPL, ResourceTemplate () {
			FixedIO (0x0060, 0x01)
			FixedIO (0x0064, 0x01)
			IRQNoFlags (IR0) {}
		})
		ENTER_CONFIG_MODE (SUPERIO_KBC_LDN)
		  PNP_WRITE_IRQ(PNP_IRQ0, Arg0, IR0)
		  PNP_DEVICE_ACTIVE = 1
		EXIT_CONFIG_MODE ()
		Notify (SUPERIO_ID(PS2, SUPERIO_KBC_LDN), 1)
	}
}

Device (SUPERIO_ID(PS2, SUPERIO_KBC_LDN)) {
	Name (_HID, EisaId ("PNP0F13"))
	Name (_UID, SUPERIO_UID(PS2, SUPERIO_KBC_LDN))

	Method (_STA)
	{
		Return (^^SUPERIO_ID(KBD, SUPERIO_KBC_LDN)._STA ())
	}

	Method (_PSC) {
		Return (^^SUPERIO_ID(KBD, SUPERIO_KBC_LDN)._PSC ())
	}

	Method (_CRS, 0, Serialized)
	{
		Name (CRS, ResourceTemplate () {
			IRQNoFlags (IR1) {}
		})
		ENTER_CONFIG_MODE (SUPERIO_KBC_LDN)
		  PNP_READ_IRQ(PNP_IRQ1, CRS, IR1)
		EXIT_CONFIG_MODE ()
		Return (CRS)
	}

	Name (_PRS, ResourceTemplate ()
	{
		StartDependentFn (0,0) {
			IRQNoFlags () {12}
		}
		EndDependentFn()
	})

	Method (_SRS, 1, Serialized)
	{
		Name (TMPL, ResourceTemplate () {
			IRQNoFlags (IR1) {}
		})
		ENTER_CONFIG_MODE (SUPERIO_KBC_LDN)
		  PNP_WRITE_IRQ(PNP_IRQ1, Arg0, IR1)
		EXIT_CONFIG_MODE ()
	}
}

#endif
}
