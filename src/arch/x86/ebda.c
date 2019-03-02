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
 */

#include <stdint.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/ebda.h>
#include <commonlib/endian.h>
#include <console/console.h>

static void *get_ebda_start(void)
{
	return (void *)((uintptr_t)DEFAULT_EBDA_SEGMENT << 4);
}

static bool is_length_valid(size_t dlength)
{
	/* Check if input data length is > DEFAULT_EBDA_SIZE */
	if (dlength > DEFAULT_EBDA_SIZE)
		return false;

	/* Valid data length */
	return true;
}

/*
 * EBDA area is representing a 1KB memory area just below
 * the top of conventional memory (below 1MB)
 */

/*
 * write_ebda_data is a wrapper function to write into EBDA area
 *
 * data = data to be written into EBDA area
 * length = input data size.
 */
void write_ebda_data(const void *data, size_t length)
{
	void *ebda;

	if (!is_length_valid(length))
		die("Input data length is > EBDA default size (1KiB)!");

	ebda = get_ebda_start();

	memcpy(ebda, data, length);
}

/*
 * read_ebda_data is a wrapper function to read from EBDA area
 *
 * data = data read from EBDA area based on input size
 * length = read data size.
 */
void read_ebda_data(void *data, size_t length)
{
	void *ebda;

	if (!is_length_valid(length))
		die("Input data length is > EBDA default size (1KiB)!");

	ebda = get_ebda_start();

	memcpy(data, ebda, length);
}

void setup_ebda(u32 low_memory_size, u16 ebda_segment, u16 ebda_size)
{
	u16 low_memory_kb;
	u16 ebda_kb;
	void *ebda;

	if (!low_memory_size || !ebda_segment || !ebda_size)
		return;

	low_memory_kb = low_memory_size >> 10;
	ebda_kb = ebda_size >> 10;
	ebda = get_ebda_start();

	/* clear BIOS DATA AREA */
	zero_n(X86_BDA_BASE, X86_BDA_SIZE);

	/* Avoid unaligned write16() since it's undefined behavior */
	write_le16(X86_EBDA_LOWMEM, low_memory_kb);
	write_le16(X86_EBDA_SEGMENT, ebda_segment);

	/* Set up EBDA */
	zero_n(ebda, ebda_size);
	write_le16(ebda, ebda_kb);
}

void setup_default_ebda(void)
{
	if (acpi_is_wakeup_s3())
		return;

	setup_ebda(DEFAULT_EBDA_LOWMEM,
		   DEFAULT_EBDA_SEGMENT,
		   DEFAULT_EBDA_SIZE);
}
