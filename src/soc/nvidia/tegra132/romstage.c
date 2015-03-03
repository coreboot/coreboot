/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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
#include <cbfs.h>
#include <console/console.h>
#include <arch/exception.h>

#include <soc/sdram_configs.h>
#include <soc/nvidia/tegra132/sdram.h>

void main(void)
{
	void *entry;

	console_init();
	exception_init();

	printk(BIOS_INFO, "T132: romstage here\n");

	sdram_init(get_sdram_config());

	printk(BIOS_INFO, "T132 romstage: sdram_init done\n");

	while (1);

	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA,
				CONFIG_CBFS_PREFIX "/ramstage");
	stage_exit(entry);
}
