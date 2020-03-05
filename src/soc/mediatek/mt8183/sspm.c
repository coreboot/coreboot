/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/barrier.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/mmio.h>
#include <soc/sspm.h>
#include <string.h>

#define BUF_SIZE (64 * KiB)
static uint8_t sspm_bin[BUF_SIZE] __aligned(8);

void sspm_init(void)
{
	const char *file_name = "sspm.bin";
	size_t fw_size = cbfs_load(file_name, sspm_bin, sizeof(sspm_bin));

	if (fw_size == 0)
		die("SSPM file :sspm.bin not found.");

	memcpy((void *)SSPM_SRAM_BASE, sspm_bin, fw_size);
	/* Memory barrier to ensure that all fw code is loaded
	   before we release the reset pin. */
	mb();
	write32(&mt8183_sspm->sw_rstn, 0x1);
}
