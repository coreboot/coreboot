/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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


#include <stdlib.h>
#include <console/console.h>
#include <arch/stages.h>
#include <cbfs.h>
#include <halt.h>
#include <program_loading.h>

void run_romstage(void)
{
	struct prog romstage = {
		.name = CONFIG_CBFS_PREFIX "/romstage",
		.type = PROG_ROMSTAGE,
	};

	if (cbfs_load_prog_stage(CBFS_DEFAULT_MEDIA, &romstage) < 0) {
		if (IS_ENABLED(CONFIG_BOOTBLOCK_CONSOLE))
			die("Couldn't load romstage.\n");
		halt();
	}

	prog_run(&romstage);
}
