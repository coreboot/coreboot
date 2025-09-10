/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/region.h>
#include <security/tpm/tspi/crtm.h>
#include <stddef.h>
#include <stdint.h>
#include <symbols.h>

#define BOOTBLOCK_BASE	(CONFIG_ROMSTAGE_ADDR - CONFIG_C_ENV_BOOTBLOCK_SIZE)

/* This region device covers the maximum range where bootblock can reside */
static const struct mem_region_device bootblock_rdev =
	MEM_REGION_DEV_RO_INIT(BOOTBLOCK_BASE, CONFIG_C_ENV_BOOTBLOCK_SIZE);


static const struct region_device *get_bootblock_rdev(void)
{
	return &bootblock_rdev.rdev;
}

static const size_t get_bootblock_base(void)
{
	return (size_t)(bootblock_rdev.base);
}

/*
 * The linker symbols _bootblock and _ebootblock provides the beginning and
 * the end of the bootblock. Use them to get the start address offset of the
 * bootblock in the RAM.
 */
static int locate_bootblock_as_rdev(struct region_device *rdev)
{
	size_t offset = (size_t)((uintptr_t)_bootblock - get_bootblock_base());
	size_t size = (size_t)((uintptr_t)_ebootblock - (uintptr_t)_bootblock);

	return rdev_chain(rdev, get_bootblock_rdev(), offset, size);
}

/*
 * On AMD non-CAR system the bootblock is stitched into the PSP BIOS directory
 * at build time. At execution time PSP loads the bootblock and uncompressed it
 * into the host RAM. Then the CPU starts to operate from this RAM as it were
 * in flash space. In order to provide a reliable CRTM init, the real executed
 * bootblock code needs to be measured into TPM if VBOOT is selected.
 */
int tspi_soc_measure_bootblock(int pcr_index)
{
	struct region_device rdev;

	if (locate_bootblock_as_rdev(&rdev))
		return 1;

	if (tpm_measure_region(&rdev, pcr_index, "bootblock"))
		return 1;

	return 0;
}
