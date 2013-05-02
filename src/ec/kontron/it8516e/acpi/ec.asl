/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * Include this file into a mainboard's DSDT _SB device tree and it will
 * expose the IT8516E in the configuration used by Kontron:
 *   2xUART,
 *   PS/2 Mouse, Keyboard
 *   Two PM Channels
 *
 * It allows the change of IO ports, IRQs and DMA settings on the devices
 * and disabling and reenabling logical devices.
 *
 * Controlled by the following preprocessor defines:
 * IT8516E_EC_DEV	Device identifier for this EC (e.g. EC0)
 * SUPERIO_PNP_BASE	I/o address of the first PnP configuration register
 * IT8516E_FIRST_DATA	I/o address of the EC_DATA register on the first
 *			pm channel
 * IT8516E_FIRST_SC	I/o address of the EC_SC register on the first
 *			pm channel
 * IT8516E_SECOND_DATA	I/o address of the EC_DATA register on the second
 *			pm channel
 * IT8516E_SECOND_SC	I/o address of the EC_SC register on the second
 *			pm channel
 */

#undef SUPERIO_CHIP_NAME
#define SUPERIO_CHIP_NAME IT8516E
#include <superio/acpi/pnp.asl>

Device(IT8516E_EC_DEV) {
	Name (_HID, EisaId("PNP0A05"))
	Name (_STR, Unicode("Kontron IT8516E Embedded Controller"))
	Name (_UID, SUPERIO_UID(IT8516E_EC_DEV,))

	/* SuperIO configuration ports */
	OperationRegion (CREG, SystemIO, SUPERIO_PNP_BASE, 0x02)
	Field (CREG, ByteAcc, NoLock, Preserve)
	{
		ADDR,   8,
		DATA,   8
	}
	IndexField (ADDR, DATA, ByteAcc, NoLock, Preserve)
	{
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
	#undef PNP_EXIT_MAGIC_1ST
	#include <superio/acpi/pnp_config.asl>

	Method (_PSC)
	{
		/* No PM: always in C0 */
		Return (0)
	}

	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_DDN
	#undef SUPERIO_UART_PM_REG
	#define SUPERIO_UART_LDN 1
	#include <superio/acpi/pnp_uart.asl>

	#undef SUPERIO_UART_LDN
	#define SUPERIO_UART_LDN 2
	#include <superio/acpi/pnp_uart.asl>

	#undef SUPERIO_KBC_LDN
	#undef SUPERIO_KBC_PS2M
	#undef SUPERIO_KBC_PS2LDN
	#define SUPERIO_KBC_LDN 6
	#define SUPERIO_KBC_PS2LDN 5
	#include <superio/acpi/pnp_kbc.asl>

	#include "pm_channels.asl"
}
