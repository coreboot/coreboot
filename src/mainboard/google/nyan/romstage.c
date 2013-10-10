/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/stages.h>
#include <device/device.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include "soc/nvidia/tegra124/chip.h"
#include <soc/display.h>

void main(void)
{
	void *entry;
	const struct device *soc;
	const struct soc_nvidia_tegra124_config *config;

	/* for quality of the user interface, it's important to get
	 * the video going ASAP. Because there are long delays in some
	 * of the powerup steps, we do some very early setup here in
	 * romstage. We don't do this in the bootblock because video
	 * setup is finicky and subject to change; hence, we do it as
	 * early as we can in the RW stage, but never in the RO stage.
	 */

	soc = dev_find_slot(DEVICE_PATH_CPU_CLUSTER, 0);
	printk(BIOS_SPEW, "s%s: soc is %p\n", __func__, soc);
	if (soc && soc->chip_info){
		config = soc->chip_info;
		setup_display((struct soc_nvidia_tegra124_config *)config);
	}

	cbmem_initialize_empty();

	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA, "fallback/coreboot_ram");
	stage_exit(entry);
}
