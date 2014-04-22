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
#include <cbmem.h>
#include <ramstage_loader.h>
#include <romstage_handoff.h>
#include <timestamp.h>

extern const struct ramstage_loader_ops cbfs_ramstage_loader;
extern const struct ramstage_loader_ops vboot_ramstage_loader;

static const struct ramstage_loader_ops *loaders[] = {
#if CONFIG_VBOOT_VERIFY_FIRMWARE
	&vboot_ramstage_loader,
#endif
	&cbfs_ramstage_loader,
};

static const char *ramstage_name = CONFIG_CBFS_PREFIX "/ramstage";
static const uint32_t ramstage_id = CBMEM_ID_RAMSTAGE;

static void
load_ramstage(const struct ramstage_loader_ops *ops, struct romstage_handoff *handoff)
{
	const struct cbmem_entry *cbmem_entry;
	void *entry_point;

	timestamp_add_now(TS_START_COPYRAM);
	entry_point = ops->load(ramstage_id, ramstage_name, &cbmem_entry);

	if (entry_point == NULL)
		return;

	cache_loaded_ramstage(handoff, cbmem_entry, entry_point);

	timestamp_add_now(TS_END_COPYRAM);

	stage_exit(entry_point);
}

static void run_ramstage_from_resume(struct romstage_handoff *handoff)
{
	void *entry;
	const struct cbmem_entry *cbmem_entry;

	if (handoff != NULL && handoff->s3_resume) {
		cbmem_entry = cbmem_entry_find(ramstage_id);

		/* No place to load ramstage. */
		if (cbmem_entry == NULL)
			return;

		/* Load the cached ramstage to runtime location. */
		entry = load_cached_ramstage(handoff, cbmem_entry);

		if (entry != NULL) {
			print_debug("Jumping to image.\n");
			stage_exit(entry);
		}
	}
}

void run_ramstage(void)
{
	struct romstage_handoff *handoff;
	const struct ramstage_loader_ops *ops;
	int i;

	handoff = romstage_handoff_find_or_add();

	run_ramstage_from_resume(handoff);

	for (i = 0; i < ARRAY_SIZE(loaders); i++) {
		ops = loaders[i];
		printk(BIOS_DEBUG, "Trying %s ramstage loader.\n", ops->name);
		load_ramstage(ops, handoff);
	}

	die("Ramstage was not loaded!\n");
}
