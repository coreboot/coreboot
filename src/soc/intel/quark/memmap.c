/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <cpu/x86/mtrr.h>
#include <soc/reg_access.h>

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;
	uintptr_t top_of_low_usable_memory;

	/* Locate the top of RAM */
	top_of_low_usable_memory = (uintptr_t)cbmem_top();
	top_of_ram = ALIGN_UP(top_of_low_usable_memory, 16 * MiB);

	/* Cache postcar and ramstage */
	postcar_frame_add_mtrr(pcf, top_of_ram - (16 * MiB), 16 * MiB,
		MTRR_TYPE_WRBACK);

	/* Cache RMU area */
	postcar_frame_add_mtrr(pcf, (uintptr_t)top_of_low_usable_memory,
		0x10000, MTRR_TYPE_WRTHROUGH);

	/* Cache ESRAM */
	postcar_frame_add_mtrr(pcf, 0x80000000, 0x80000, MTRR_TYPE_WRBACK);

	pcf->skip_common_mtrr = 1;
	/* Cache SPI flash - Write protect not supported */
	postcar_frame_add_romcache(pcf, MTRR_TYPE_WRTHROUGH);
}
