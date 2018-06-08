/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Lubomir Rintel <lkundrak@v3.sk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* VT8237 ISA bridge */
Device (ISAC)
{
	Name (_ADR, 0x00110000)

	OperationRegion (ISAB, PCI_Config, 0x00, 0xEF)
	Field (ISAB, DWordAcc, NoLock, Preserve) {

		/* PCI PNP Interrupt Routing 1 */
		Offset (0x55),
		    , 4,	/* Reserved */
		INTA, 4,	/* PCI INTA# Routing */

		/* PCI PNP Interrupt Routing 2 */
		Offset (0x56),
		INTB, 4,	/* PCI INTB# Routing */
		INTC, 4,	/* PCI INTC# Routing */

		/* PCI PNP Interrupt Routing 3 */
		Offset (0x57),
		    , 4,	/* Reserved */
		INTD, 4,	/* PCI INTD# Routing */

		/* Miscellaneous Control 0 */
		Offset (0x58),
		    , 6,
		APIC, 1,	/* Internal APIC Enable */
	}

	#include "irqlinks.asl"
}
