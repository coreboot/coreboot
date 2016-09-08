/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <arch/io.h>
#include <console/console.h>
#include <soc/clock.h>
#include <soc/flash.h>

struct flash_ops flash_callbacks = {
	.init = (void *)MVMAP2315_FLASH_INIT,
	.read = (void *)MVMAP2315_FLASH_READ,
	.write = (void *)MVMAP2315_FLASH_WRITE,
	.shutdown = (void *)MVMAP2315_FLASH_SHUTDOWN,
	.set_partition = (void *)MVMAP2315_FLASH_SET_PARTITION,
};

u32 flash_init(u32 media, u32 clock_input_mhz)
{
	int rc;

	clrbits_le32(&mvmap2315_apmu_clk->apaonclk_sdmmc_clkgenconfig,
		     MVMAP2315_SDMMC_CLK_RSTN);
	setbits_le32(&mvmap2315_apmu_clk->apaonclk_sdmmc_clkgenconfig,
		     MVMAP2315_SDMMC_CLK_RSTN);

	rc = flash_callbacks.init(media, 0, clock_input_mhz);

	if (rc)
		printk(BIOS_DEBUG, "flash_init failed with rc=%x.\n", rc);

	return rc;
}

u32 flash_partition(u32 media, struct flash_params *flash_image_info)
{
	int rc;

	rc = flash_callbacks.set_partition(media, 0, flash_image_info);

	if (rc)
		printk(BIOS_DEBUG, "flash_partition failed with rc=%x.\n", rc);

	return rc;
}

u32 flash_read(u32 media, struct flash_params *flash_image_info)
{
	int rc;

	rc = flash_callbacks.read(media, 0, flash_image_info);

	if (rc)
		printk(BIOS_DEBUG, "flash_read failed with rc=%x.\n", rc);

	return rc;
}

u32 flash_write(u32 media, struct flash_params *flash_image_info)
{
	int rc;

	rc = flash_callbacks.write(media, 0, flash_image_info);

	if (rc)
		printk(BIOS_DEBUG, "flash_write failed with rc=%x.\n", rc);

	return rc;
}

u32 flash_shutdown(u32 media)
{
	int rc;

	rc = flash_callbacks.shutdown(media, 0, 0);

	if (rc)
		printk(BIOS_DEBUG, "flash_shutdown failed with rc=%x.\n", rc);

	return rc;
}
