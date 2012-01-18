/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <arch/ebda.h>
#if CONFIG_HAVE_ACPI_RESUME
#include <arch/acpi.h>
#endif

void setup_ebda(u32 low_memory_size, u16 ebda_segment, u16 ebda_size)
{
#if CONFIG_HAVE_ACPI_RESUME
	/* Skip in S3 resume path */
	if (acpi_slp_type == 3)
		return;
#endif

	if (!low_memory_size || !ebda_segment || !ebda_size)
		return;

	/* clear BIOS DATA AREA */
	memset((void *)X86_BDA_BASE, 0, X86_BDA_SIZE);

	write16(X86_EBDA_LOWMEM, (low_memory_size >> 10));
	write16(X86_EBDA_SEGMENT, ebda_segment);

	/* Set up EBDA */
	memset((void *)(ebda_segment << 4), 0, ebda_size);
	write16((ebda_segment << 4), (ebda_size >> 10));
}

void setup_default_ebda(void)
{
	setup_ebda(DEFAULT_EBDA_LOWMEM,
		   DEFAULT_EBDA_SEGMENT,
		   DEFAULT_EBDA_SIZE);
}
