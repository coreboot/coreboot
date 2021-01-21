/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20111103	// OEM revision
) {
	#include <acpi/dsdt_top.asl>

Scope(\_SB)
{
	Device(PCI0) {
		Name (_HID, EISAID("PNP0A03"))
		Name (_PRT, Package() {
			Package() { 0x001dffff, 0, 0, 16 },
			Package() { 0x001dffff, 1, 0, 19 },
			Package() { 0x001dffff, 2, 0, 18 },
			Package() { 0x001dffff, 3, 0, 23 },
			Package() { 0x001fffff, 0, 0, 18 },
			Package() { 0x001fffff, 1, 0, 17 },
		})

		#include "acpi/e7505_sec.asl"

		OperationRegion (I750, PCI_Config, 0x00, 0x0100)
		Field (I750, ByteAcc, NoLock, Preserve)
		{
			Offset (0xC4),
				TOLM,   16,	/* Top of Low Memory */
				RBAR,   16,	/* REMAP_BASE */
				RLAR,   16	/* REMAP_LIMIT */
		}
	}

	#include "acpi/e7505_pri.asl"


	Device (PWBT)
	{
		Name (_HID, EisaId ("PNP0C0C"))
		Name (_PRW, Package () { 0x08, 0x05 })
	}

	Device (SLBT)
	{
		Name (_HID, EisaId ("PNP0C0E"))
		Name (_PRW, Package () { 0x0B, 0x05 })
	}

	Device (LID0)
	{
		Name (_HID, EisaId ("PNP0C0D"))
		Name (_PRW, Package () { 0x0B, 0x05 })
	}

}

Scope(\_SB.PCI0)
{

	Device(PCI1)
	{
		Name (_ADR, 0x00010000)
		Name (_PRT, Package() {
			Package() { 0x0000ffff, 0, 0, 16 },
			Package() { 0x0000ffff, 1, 0, 17 },
		})
	}

	Device(HLIB)
	{
		Name (_ADR, 0x00020000)
		Name (_PRT, Package() {
			Package() { 0x001dffff, 0, 0, 18 },
			Package() { 0x001dffff, 1, 0, 18 },
			Package() { 0x001dffff, 2, 0, 18 },
			Package() { 0x001dffff, 3, 0, 18 },
			Package() { 0x001fffff, 0, 0, 18 },
			Package() { 0x001fffff, 1, 0, 18 },
			Package() { 0x001fffff, 2, 0, 18 },
			Package() { 0x001fffff, 3, 0, 18 },
		})

		#include "acpi/p64h2.asl"
	}

	#include "acpi/i82801db.asl"
}

#include "acpi/power.asl"

}
