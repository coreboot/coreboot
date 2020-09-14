/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cbmem.h>
#include <console/console.h>
#include <fsp/util.h>

void fsp_verify_memory_init_hobs(void)
{
	struct range_entry fsp_mem;
	struct range_entry tolum;

	/* Verify the size of the TOLUM range */
	fsp_find_bootloader_tolum(&tolum);
	if (range_entry_size(&tolum) < cbmem_overhead_size()) {
		printk(BIOS_CRIT,
			"FSP_BOOTLOADER_TOLUM_SIZE: 0x%08llx < 0x%08zx\n",
			range_entry_size(&tolum), cbmem_overhead_size());
		die("FSP_BOOTLOADER_TOLUM_HOB too small!\n");
	}

	/* Verify the bootloader tolum is above the FSP reserved area */
	fsp_find_reserved_memory(&fsp_mem);
	if (range_entry_end(&tolum) <= range_entry_base(&fsp_mem)) {
		printk(BIOS_CRIT,
			"TOLUM end: 0x%08llx != 0x%08llx: FSP rsvd base\n",
			range_entry_end(&tolum), range_entry_base(&fsp_mem));
		die("FSP reserved region after BIOS TOLUM!\n");
	}
	if (range_entry_base(&tolum) < range_entry_end(&fsp_mem)) {
		printk(BIOS_CRIT,
			"TOLUM base: 0x%08llx < 0x%08llx: FSP rsvd end\n",
			range_entry_base(&tolum), range_entry_end(&fsp_mem));
		die("FSP reserved region overlaps BIOS TOLUM!\n");
	}

	/* Verify that the FSP reserved area immediately follows the BIOS
	 * reserved area
	 */
	if (range_entry_base(&tolum) != range_entry_end(&fsp_mem)) {
		printk(BIOS_CRIT,
			"TOLUM base: 0x%08llx != 0x%08llx: FSP rsvd end\n",
			range_entry_base(&tolum), range_entry_end(&fsp_mem));
		die("Space between FSP reserved region and BIOS TOLUM!\n");
	}

	if (!CONFIG(ACPI_BERT) && range_entry_end(&tolum) != (uintptr_t)cbmem_top()) {
		printk(BIOS_CRIT, "TOLUM end: 0x%08llx != %p: cbmem_top\n",
			range_entry_end(&tolum), cbmem_top());
		die("Space between cbmem_top and BIOS TOLUM!\n");
	}

	if (CONFIG(ACPI_BERT) &&
		range_entry_end(&tolum) != (uintptr_t)cbmem_top() + CONFIG_ACPI_BERT_SIZE) {
		printk(BIOS_CRIT, "TOLUM end: 0x%08llx != %p: cbmem_top + 0x%x: BERT\n",
			range_entry_end(&tolum), cbmem_top(), CONFIG_ACPI_BERT_SIZE);
		die("Space between cbmem_top and APEI BERT!\n");
	}
}
