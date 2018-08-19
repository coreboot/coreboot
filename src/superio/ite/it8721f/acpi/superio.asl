/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Christoph Grenz <christophg+cb@grenz-bonn.de>
 * Copyright (C) 2013, 2016 secunet Security Networks AG
 * Copyright (C) 2017 Samuel Holland <samuel@sholland.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Include this file into a mainboard's DSDT _SB device tree and it will
 * expose the IT8721F SuperIO and some of its functionality.
 *
 * It allows the change of IO ports, IRQs and DMA settings on logical
 * devices, disabling and reenabling logical devices.
 *
 *   LDN		State
 * 0x0 FDC		Not implemented
 * 0x1 SP1		Implemented
 * 0x2 SP2		Implemented
 * 0x3 PP		Not implemented
 * 0x4 EC		Implemented, untested
 * 0x5 KBCK		Implemented, untested
 * 0x6 KBCM		Implemented, untested
 * 0x7 GPIO		Not implemented
 * 0xa CIR		Not implemented
 *
 * Controllable through preprocessor defines:
 * SUPERIO_DEV		Device identifier for this SIO (e.g. SIO0)
 * SUPERIO_PNP_BASE	I/O address of the first PnP configuration register
 * IT8721F_SHOW_SP1	If defined, Serial Port 1 will be exposed.
 * IT8721F_SHOW_SP2	If defined, Serial Port 2 will be exposed.
 * IT8721F_SHOW_EC	If defined, the Environment Controller will be exposed.
 * IT8721F_SHOW_KBCK	If defined, the Keyboard Controller will be exposed.
 * IT8721F_SHOW_KBCM	If defined, PS/2 mouse support will be exposed.
 * IT8721F_SHOW_GPIO	If defined, GPIO support will be exposed.
 * IT8721F_SHOW_CIR	If defined, Consumer IR support will be exposed.
 */

#undef SUPERIO_CHIP_NAME
#define SUPERIO_CHIP_NAME IT8721F
#include <superio/acpi/pnp.asl>

#undef PNP_DEFAULT_PSC
#define PNP_DEFAULT_PSC Return (0) /* no power management */

#define CONFIGURE_CONTROL CCTL

Device(SUPERIO_DEV) {
	Name (_HID, EisaId("PNP0A05"))
	Name (_STR, Unicode("ITE IT8721F Super I/O"))
	Name (_UID, SUPERIO_UID(SUPERIO_DEV,))

	/* SuperIO configuration ports */
	OperationRegion (CREG, SystemIO, SUPERIO_PNP_BASE, 0x02)
	Field (CREG, ByteAcc, NoLock, Preserve)
	{
		PNP_ADDR_REG,	8,
		PNP_DATA_REG,   8
	}
	IndexField (ADDR, DATA, ByteAcc, NoLock, Preserve)
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
		Offset (0x62),
		PNP_IO1_HIGH_BYTE,	8, /* Second I/O port base - high byte */
		PNP_IO1_LOW_BYTE,	8, /* Second I/O port base - low byte */
		Offset (0x64),
		PNP_IO2_HIGH_BYTE,	8, /* Third I/O port base - high byte */
		PNP_IO2_LOW_BYTE,	8, /* Third I/O port base - low byte */

		Offset (0x70),
		PNP_IRQ0,		8, /* First IRQ */
	}

	Method (_CRS)
	{
		/* Announce the used I/O ports to the OS */
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

#ifdef IT8721F_SHOW_SP1
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_DDN
	#undef SUPERIO_UART_PM_REG
	#undef SUPERIO_UART_PM_VAL
	#undef SUPERIO_UART_PM_LDN
	#define SUPERIO_UART_LDN 1
	#include <superio/acpi/pnp_uart.asl>
#endif

#ifdef IT8721F_SHOW_SP2
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_DDN
	#undef SUPERIO_UART_PM_REG
	#undef SUPERIO_UART_PM_VAL
	#undef SUPERIO_UART_PM_LDN
	#define SUPERIO_UART_LDN 2
	#include <superio/acpi/pnp_uart.asl>
#endif

#ifdef IT8721F_SHOW_EC
	#undef SUPERIO_PNP_HID
	#undef SUPERIO_PNP_LDN
	#undef SUPERIO_PNP_DDN
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
	#define SUPERIO_PNP_IO0 0x08, 0x08
	#define SUPERIO_PNP_IO1 0x08, 0x04
	#define SUPERIO_PNP_IRQ0
	#include <superio/acpi/pnp_generic.asl>
#endif

#ifdef IT8721F_SHOW_KBCK
	#undef SUPERIO_KBC_LDN
	#undef SUPERIO_KBC_PS2M
	#undef SUPERIO_KBC_PS2LDN
	#define SUPERIO_KBC_LDN 5
#ifdef IT8721F_SHOW_KBCM
	#define SUPERIO_KBC_PS2LDN 6
#endif
	#include <superio/acpi/pnp_kbc.asl>
#endif
}
