/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/region.h>
#include <security/tpm/tspi/crtm.h>
#include <soc/iomap.h>
#include <stddef.h>
#include <stdint.h>
#include <symbols.h>

extern const uint64_t fit_ptr;
/* This region device covers the shared SRAM that gets mapped at bootblock runtime. */
static const struct mem_region_device sram_rdev =
		MEM_REGION_DEV_RO_INIT(SHARED_SRAM_BASE, SHARED_SRAM_SIZE);


static const struct region_device *shared_sram_device(void)
{
	return &sram_rdev.rdev;
}

static const size_t shared_sram_base(void)
{
	return (size_t)(sram_rdev.base);
}

/*
 * The linker symbol _program provides the beginning of the .text section in the bootblock.
 * Use it to get the start address offset of the bootblock in the shared SRAM.
 */
static int ifwi_bootblock_locate_as_rdev(struct region_device *rdev)
{
	size_t offset = (size_t)(_program - shared_sram_base());
	size_t size = (size_t)(0x100000000ULL - (uint32_t)_program);

	return rdev_chain(rdev, shared_sram_device(), offset, size);
}

/*
 * On Apollo Lake the bootblock is stitched into the IBBL IFWI region at
 * build time. At execution time TXE loads this IBBL into a shared SRAM
 * (which is read-only in this phase) and maps it at 4 GiB - 32 KiB.
 * Then the CPU starts to operate from this shared SRAM as it were flash space.
 * In order to provide a reliable CRTM init, the real executed bootblock
 * code needs to be measured into TPM if VBOOT is selected.
 */
int tspi_soc_measure_bootblock(int pcr_index)
{
	struct region_device ifwi_bootblock;

	if (ifwi_bootblock_locate_as_rdev(&ifwi_bootblock))
		return 1;
	if (tpm_measure_region(&ifwi_bootblock, pcr_index, "IFWI: bootblock"))
		return 1;
	printk(BIOS_DEBUG, "FIT pointer patched to 0x%llx by TXE.\n", fit_ptr);
	/* Check if the patched FIT pointer address  matches the pre-defined one. */
	if (fit_ptr != SHARED_SRAM_BASE) {
		printk(BIOS_WARNING,
			"Runtime FIT pointer does not match the pre-defined address!\n");
	}
	return 0;
}
