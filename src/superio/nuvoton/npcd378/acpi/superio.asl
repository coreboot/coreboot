/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Christoph Grenz <christophg+cb@grenz-bonn.de>
 * Copyright (C) 2013, 2016 secunet Security Networks AG
 * Copyright (C) 2017 Tobias Diedrich <ranma+coreboot@tdiedrich.de>
 * Copyright (C) 2018 Patrick Rudolph <siro@das-labor.org>
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
 * expose the NPCD378 SuperIO and some of its functionality.
 *
 * It allows the change of IO ports, IRQs and DMA settings on logical
 * devices, disabling and reenabling logical devices.
 *
 *   LDN		State
 * 0x2 SP1		Implemented, untested
 * 0x5 KBCK		Implemented, untested
 */

#undef SUPERIO_CHIP_NAME
#define SUPERIO_CHIP_NAME NPCD378
#include <superio/acpi/pnp.asl>

#undef PNP_DEFAULT_PSC
#define PNP_DEFAULT_PSC Return (0) /* no power management */

Device(SUPERIO_DEV) {
	Name (_HID, EisaId("PNP0A05"))
	Name (_STR, Unicode("Nuvoton NPCD378 Super I/O"))
	Name (_UID, SUPERIO_UID(SUPERIO_DEV,))

	/* SuperIO configuration ports */
	OperationRegion (CREG, SystemIO, SUPERIO_PNP_BASE, 0x02)
	Field (CREG, ByteAcc, NoLock, Preserve)
	{
		PNP_ADDR_REG,	8,
		PNP_DATA_REG,	8,
	}
	IndexField (ADDR, DATA, ByteAcc, NoLock, Preserve)
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
	}

	Method (_CRS)
	{
		/* Announce the used I/O ports to the OS */
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

#ifdef SUPERIO_SHOW_LPT
	#undef SUPERIO_PNP_HID
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
	#undef PNP_DEVICE_ACTIVE
	#define PNP_DEVICE_ACTIVE ACT3
	#define SUPERIO_PNP_LDN 1
	#define SUPERIO_PNP_IO0 0x08, 0x08
	#include <superio/acpi/pnp_generic.asl>
#endif

#ifdef SUPERIO_SHOW_SP1
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_DDN
	#undef SUPERIO_UART_PM_REG
	#undef SUPERIO_UART_PM_VAL
	#undef SUPERIO_UART_PM_LDN
	#define SUPERIO_UART_LDN 2
	#include <superio/acpi/pnp_uart.asl>
#endif

#ifdef SUPERIO_SHOW_SP2
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_DDN
	#undef SUPERIO_UART_PM_REG
	#undef SUPERIO_UART_PM_VAL
	#undef SUPERIO_UART_PM_LDN
	#define SUPERIO_UART_LDN 3
	#include <superio/acpi/pnp_uart.asl>
#endif

#ifdef SUPERIO_SHOW_KBC
	#undef SUPERIO_KBC_LDN
	#undef SUPERIO_KBC_PS2M
	#undef SUPERIO_KBC_PS2LDN
	#define SUPERIO_KBC_PS2LDN 5
	#define SUPERIO_KBC_LDN 6
	#include <superio/acpi/pnp_kbc.asl>
#endif

#ifdef SUPERIO_SHOW_GPIO
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
	#undef PNP_DEVICE_ACTIVE
	#define PNP_DEVICE_ACTIVE ACT3
	#define SUPERIO_PNP_LDN 8
	#define SUPERIO_PNP_IO0 0x08, 0x08
	#include <superio/acpi/pnp_generic.asl>
#endif
}
