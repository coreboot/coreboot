/*
 * coreboot ACPI Table support
 * written by Stefan Reinauer <stepan@openbios.org>
 * ACPI FADT, FACS, and DSDT table support added by
 * Nick Barker <nick.barker9@btinternet.com>, and those portions
 * (C) Copyright 2004 Nick Barker
 * (C) Copyright 2005 Stefan Reinauer
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>

unsigned long acpi_fill_mcfg(unsigned long current)
{
	/* Nothing to do */
	return current;
}

unsigned long acpi_fill_slit(unsigned long current)
{
	// Not implemented
	return current;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Nothing to do */
	return current;
}

unsigned long acpi_fill_srat(unsigned long current)
{
	/* No NUMA, no SRAT */
	return current;
}
