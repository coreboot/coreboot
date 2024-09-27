/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/ioapic.h>
#include <cpu/x86/lapic_def.h>
#include <soc/iomap.h>

Scope (\_SB.PCI0)
{
	/* PCH Reserved resources. */
	Device (PCHR)
	{
		Name (_HID, EisaId ("PNP0C02"))
		Name (_CRS, ResourceTemplate ()
		{
			Memory32Fixed (ReadWrite, RESERVED_BASE_ADDRESS, RESERVED_BASE_SIZE)
		})
	}

	Device (APIC)
	{
		Name (_HID, EisaId ("PNP0003"))
		Name (_CRS, ResourceTemplate ()
		{
			/* IO APIC */
			Memory32Fixed (ReadOnly, IO_APIC_ADDR, 0x00100000)
			/* Local APIC */
			Memory32Fixed (ReadOnly, LAPIC_DEFAULT_BASE, 0x00100000)
		})
	}

	#include <soc/intel/common/block/acpi/acpi/lpc.asl>

	Device (PMC)
	{
		Name (_HID, EISAID ("PNP0C02"))
		Name (_DDN, "PCH PMC")
		Name (_UID, "PMC")

		Name (_CRS, ResourceTemplate
		{
			IO (Decode16, ACPI_BASE_ADDRESS, ACPI_BASE_ADDRESS, 0x1, 0x80)
			Memory32Fixed (ReadOnly, PCH_PWRM_BASE_ADDRESS, 0x10000)
		})
	}
}
