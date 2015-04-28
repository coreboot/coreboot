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
#include <timestamp.h>

extern const struct prog_loader_ops cbfs_romstage_loader;

static const struct prog_loader_ops *loaders[] = {
	&cbfs_romstage_loader,
};

void run_romstage(void)
{
	int i;
	struct prog romstage = {
		.name = CONFIG_CBFS_PREFIX "/romstage",
		.type = PROG_ROMSTAGE,
	};

	for (i = 0; i < ARRAY_SIZE(loaders); i++) {
		/* Default loader state is active. */
		int ret = 1;
		const struct prog_loader_ops *ops;

		ops = loaders[i];

		if (ops->is_loader_active != NULL)
			ret = ops->is_loader_active(&romstage);

		if (ret == 0) {
			printk(BIOS_DEBUG, "%s romstage loader inactive.\n",
				ops->name);
			continue;
		} else if (ret < 0) {
			printk(BIOS_DEBUG, "%s romstage loader failure.\n",
				ops->name);
			continue;
		}

		printk(BIOS_DEBUG, "%s romstage loader active.\n", ops->name);

		timestamp_add_now(TS_START_COPYROM);

		if (ops->prepare(&romstage))
			continue;

		timestamp_add_now(TS_END_COPYROM);

		prog_run(&romstage);
	}

	if (IS_ENABLED(CONFIG_BOOTBLOCK_CONSOLE))
		die("Couldn't load romstage.\n");
	halt();
}
