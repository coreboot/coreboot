/* SPDX-License-Identifier: GPL-2.0-only */

/* Enable ACPI _SWS methods */

/* Port 80 POST */
OperationRegion (DBG0, SystemIO, 0x80, 0x02)
Field (DBG0, ByteAcc, Lock, Preserve)
{
	IO80, 8,
	IO81, 8
}
