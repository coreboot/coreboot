/*
 * This file is part of the coreboot project.
 *
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

/* Enable ACPI _SWS methods */
#include <soc/intel/common/acpi/acpi_wake_source.asl>
/* Generic indicator for sleep state */
#include <soc/intel/common/acpi/platform.asl>

/*
 * The _PIC method is called by the OS to choose between interrupt
 * routing via the i8259 interrupt controller or the APIC.
 *
 * _PIC is called with a parameter of 0 for i8259 configuration and
 * with a parameter of 1 for Local Apic/IOAPIC configuration.
 */

Method (_PIC, 1)
{
	/* Remember the OS' IRQ routing choice. */
	Store (Arg0, PICM)
}
