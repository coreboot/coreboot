/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Kyösti Mälkki <kyosti.malkki@gmail.com>
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

/* PCI-X devices 04:04.0 and 04:04.1 : AIC-7902W
 * U320 SCSI dual-channel controller
 */

Device (SCS0)
{
	Name (_ADR, 0x00040000)
	OperationRegion (SCSC, PCI_Config, 0x00, 0x0100)
	Field (SCSC, ByteAcc, NoLock, Preserve)
	{
		Offset (0x2C),   SID,   32,
		Offset (0xE0),   PMC,   8,
		Offset (0xFF),   IDW,   8
	}
}

Device (SCS1)
{
	Name (_ADR, 0x00040001)
	OperationRegion (SCSC, PCI_Config, 0x00, 0x0100)
	Field (SCSC, ByteAcc, NoLock, Preserve)
	{
		Offset (0x2C),   SID,   32,
		Offset (0xE0),   PMC,   8,
		Offset (0xFF),   IDW,   8
	}
}

#if 0
/* Set subsystem id for both SCSI devices.
 * It may require some delay on wake-up before this can be done.
 */
	Method ( )
	{
		Or (\_SB.PCI0.HLIB.P64A.SCS0.IDW, 0x01, \_SB.PCI0.HLIB.P64A.SCS0.IDW)
		Store (0x1106A0A0, \_SB.PCI0.HLIB.P64A.SCS0.SID)
		And (\_SB.PCI0.HLIB.P64A.SCS0.IDW, 0xFE, \_SB.PCI0.HLIB.P64A.SCS0.IDW)

		Or (\_SB.PCI0.HLIB.P64A.SCS1.IDW, 0x01, \_SB.PCI0.HLIB.P64A.SCS1.IDW)
		Store (0x1106A0A0, \_SB.PCI0.HLIB.P64A.SCS1.SID)
		And (\_SB.PCI0.HLIB.P64A.SCS1.IDW, 0xFE, \_SB.PCI0.HLIB.P64A.SCS1.IDW)
	}
#endif

