/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <soc/iomap.h>

#define MAILBOX_DATA 0x7080
#define MAILBOX_INTF 0x7084
#define PMIO_LENGTH 0x80
#define PMIO_LIMIT 0x480

scope (\_SB) {
	Device (IPC1)
	{
		Name (_HID, "INT34D2")
		Name (_CID, "INT34D2")
		Name (_DDN, "Intel(R) IPC1 Controller")
		Name (RBUF, ResourceTemplate ()
		{
			Memory32Fixed (ReadWrite, 0x0, 0x2000, IBAR)
			Memory32Fixed (ReadWrite, 0x0, 0x4, MDAT)
			Memory32Fixed (ReadWrite, 0x0, 0x4, MINF)
			IO (Decode16, ACPI_PMIO_BASE, PMIO_LIMIT,
			      0x04, PMIO_LENGTH)
			Memory32Fixed (ReadWrite, 0x0, 0x2000, SBAR)
			Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive, , , )
			{
			      PMC_INT
			}
		})

		Method (_CRS, 0x0, NotSerialized)
		{
			CreateDwordField (^RBUF, ^IBAR._BAS, IBAS)
			Store (PMC_BAR0, IBAS)

			CreateDwordField (^RBUF, ^MDAT._BAS, MDBA)
			Store (MCH_BASE_ADDRESS + MAILBOX_DATA, MDBA)
			CreateDwordField (^RBUF, ^MINF._BAS, MIBA)
			Store (MCH_BASE_ADDRESS + MAILBOX_INTF, MIBA)

			CreateDwordField (^RBUF, ^SBAR._BAS, SBAS)
			Store (PMC_SRAM_BASE_0, SBAS)

			Return (^RBUF)
		}
	}
}