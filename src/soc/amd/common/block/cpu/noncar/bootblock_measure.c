/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/region.h>
#include <security/tpm/tspi/crtm.h>
#include <stddef.h>
#include <stdint.h>
#include <symbols.h>

/*
 * On AMD non-CAR system the bootblock is stitched into the PSP BIOS directory
 * at build time. At execution time PSP loads the bootblock and uncompressed it
 * into the host RAM. Then the CPU starts to operate from this RAM as it were
 * in flash space. In order to provide a reliable CRTM init, the real executed
 * bootblock code needs to be measured into TPM if VBOOT is selected.
 */
int tspi_soc_measure_bootblock(int pcr_index)
{
#if ENV_X86
	const struct mem_region_device bootblock_rdev =
		MEM_REGION_DEV_RO_INIT(_bootblock, _ebootblock - _bootblock);

	if (tpm_measure_region(&bootblock_rdev.rdev, pcr_index, "bootblock"))
		return 1;
	return 0;
#else
	return 1;
#endif
}
