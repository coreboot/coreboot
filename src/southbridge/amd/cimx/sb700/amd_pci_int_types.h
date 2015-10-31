/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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

#ifndef AMD_PCI_INT_TYPES_H
#define AMD_PCI_INT_TYPES_H

const char * intr_types[] = {
	[0x00] = "INTA#\t", "INTB#\t", "INTC#\t", "INTD#\t",
	[0x04] = "ACPI\t", "SMBUS\t", "RSVD\t", "RSVD\t", "RSVD\t",
	[0x09] = "INTE#\t", "INTF#\t", "INTG#\t", "INTH#\t",
};

#endif /* AMD_PCI_INT_TYPES_H */
