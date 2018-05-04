/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
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
#include <arch/io.h>
#include <symbols.h>
#include <console/console.h>
#include <device/device.h>
#include <boot/coreboot_tables.h>

#include <vendorcode/google/chromeos/chromeos.h>

static void mainboard_init(struct device *dev)
{
#if IS_ENABLED(CONFIG_CHROMEOS)
	/* Copy WIFI calibration data into CBMEM. */
	cbmem_add_vpd_calibration_data();
#endif
}

static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Enable Pistachio device...\n");
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

void lb_board(struct lb_header *header)
{
	struct lb_range *dma;

	dma = (struct lb_range *)lb_new_record(header);
	dma->tag = LB_TAB_DMA;
	dma->size = sizeof(*dma);
	dma->range_start = (uintptr_t)_dma_coherent;
	dma->range_size = _dma_coherent_size;

#if IS_ENABLED(CONFIG_CHROMEOS)
	/* Retrieve the switch interface MAC addresses. */
	lb_table_add_macs_from_vpd(header);
#endif
}
