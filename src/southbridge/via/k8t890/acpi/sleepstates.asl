/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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

/* S1 support: bit 0, S2 Support: bit 1, etc. S0 & S5 assumed */
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
Name (SSFG, 0x04)
#else
Name (SSFG, 0x00)
#endif

/* Supported sleep states: */
Name(\_S0, Package () {0x00, 0x00, 0x00, 0x00} )	/* (S0) - working state */

If (And(SSFG, 0x04)) {
	Name(\_S3, Package () {0x01, 0x01, 0x00, 0x00} )	/* (S3) - Suspend to RAM */
}

Name(\_S5, Package () {0x02, 0x02, 0x00, 0x00} )	/* (S5) - Soft Off */
