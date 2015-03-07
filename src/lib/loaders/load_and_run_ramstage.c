/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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
#include <program_loading.h>
#include <romstage_handoff.h>
#include <stage_cache.h>
#include <timestamp.h>

extern const struct prog_loader_ops cbfs_ramstage_loader;
extern const struct prog_loader_ops vboot_ramstage_loader;

static const struct prog_loader_ops *loaders[] = {
#if CONFIG_VBOOT_VERIFY_FIRMWARE
	&vboot_ramstage_loader,
#endif
	&cbfs_ramstage_loader,
};

void __attribute__((weak)) stage_cache_add(int stage_id, struct prog *stage) {}
void __attribute__((weak)) stage_cache_load_stage(int stage_id,
							struct prog *stage) {}
void __attribute__((weak)) ramstage_cache_invalid(void) {}

static void load_ramstage(const struct prog_loader_ops *ops,
				struct prog *ramstage)
{
	timestamp_add_now(TS_START_COPYRAM);

	if (ops->prepare(ramstage))
		return;

	stage_cache_add(STAGE_RAMSTAGE, ramstage);

	timestamp_add_now(TS_END_COPYRAM);

	prog_run(ramstage);
}

static void run_ramstage_from_resume(struct romstage_handoff *handoff,
					struct prog *ramstage)
{
	if (handoff != NULL && handoff->s3_resume) {
		/* Load the cached ramstage to runtime location. */
		stage_cache_load_stage(STAGE_RAMSTAGE, ramstage);

		if (prog_entry(ramstage) != NULL) {
			printk(BIOS_DEBUG, "Jumping to image.\n");
			prog_run(ramstage);
		}
		ramstage_cache_invalid();
	}
}

void run_ramstage(void)
{
	const struct prog_loader_ops *ops;
	int i;
	struct prog ramstage = {
		.name = CONFIG_CBFS_PREFIX "/ramstage",
		.type = PROG_RAMSTAGE,
	};

	run_ramstage_from_resume(romstage_handoff_find_or_add(), &ramstage);

	for (i = 0; i < ARRAY_SIZE(loaders); i++) {
		ops = loaders[i];
		printk(BIOS_DEBUG, "Trying %s ramstage loader.\n", ops->name);
		load_ramstage(ops, &ramstage);
	}

	die("Ramstage was not loaded!\n");
}
