/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <acpi/acpi.h>
#include <arch/ebda.h>
#include <commonlib/endian.h>

static void *get_ebda_start(void)
{
	return (void *)((uintptr_t)DEFAULT_EBDA_SEGMENT << 4);
}

/*
 * EBDA area is representing a 1KB memory area just below
 * the top of conventional memory (below 1MB)
 */

static void setup_ebda(u32 low_memory_size, u16 ebda_segment, u16 ebda_size)
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
