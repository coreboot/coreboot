/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

// Intel PMC Controller 0:1f.2

Device (PMC0)
{
	Name (_ADR, 0x001F0002)

	Device(PDRC)	// PMC Device Resource Consumption
	{
		Name(_HID,EISAID("PNP0C02"))
		Name(_UID, 0x10)

		Name(PMCR,ResourceTemplate()
		{
			IO (Decode16, DEFAULT_PMBASE, DEFAULT_PMBASE, 0x1, 0x80)	// ACPI Base
			Memory32Fixed(ReadOnly, DEFAULT_PWRM_BASE, 0x1000) // PMC memory range
		})

		Method(_CRS, 0)
		{
			return(PMCR)
		}
	}
}
