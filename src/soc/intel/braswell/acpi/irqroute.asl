/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2013 Google Inc.
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

/* PCI Interrupt Routing */
Method(_PRT)
{
	If (PICM) {
		Return (Package() {
			#undef PIC_MODE
			#include <soc/intel/braswell/acpi/irq_helper.h>
			PCI_DEV_PIRQ_ROUTES
		})
	} Else {
		Return (Package() {
			#define PIC_MODE
			#include <soc/intel/braswell/acpi/irq_helper.h>
			PCI_DEV_PIRQ_ROUTES
		})
	}
}
