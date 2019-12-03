/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 MediaTek Inc.
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
	size_t fw_size = cbfs_boot_load_file(file_name,
					     sspm_bin,
					     sizeof(sspm_bin),
					     CBFS_TYPE_RAW);

	if (fw_size == 0)
		die("SSPM file :sspm.bin not found.");

	memcpy((void *)SSPM_SRAM_BASE, sspm_bin, fw_size);
	/* Memory barrier to ensure that all fw code is loaded
	   before we release the reset pin. */
	mb();
	write32(&mt8183_sspm->sw_rstn, 0x1);
}
