/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
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
#include <console/console.h>
#include <cbfs.h>
#include <arch/stages.h>
#include <ramstage_loader.h>
#include <timestamp.h>

#if CONFIG_RELOCATABLE_RAMSTAGE
#include <rmodule.h>

static void *cbfs_load_ramstage(uint32_t cbmem_id, const char *name,
				const struct cbmem_entry **cbmem_entry)
{
	struct rmod_stage_load rmod_ram = {
		.cbmem_id = cbmem_id,
		.name = name,
	};

	if (rmodule_stage_load_from_cbfs(&rmod_ram)) {
		printk(BIOS_DEBUG, "Could not load ramstage.\n");
		return NULL;
	}

	*cbmem_entry = rmod_ram.cbmem_entry;

	return rmod_ram.entry;
}

#else /* CONFIG_RELOCATABLE_RAMSTAGE */

static void *cbfs_load_ramstage(uint32_t cbmem_id, const char *name,
				const struct cbmem_entry **cbmem_entry)
{
	void *entry;

	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA, name);

	if ((void *)entry == (void *) -1)
		entry = NULL;

	return entry;
}

#endif /* CONFIG_RELOCATABLE_RAMSTAGE */

const struct ramstage_loader_ops cbfs_ramstage_loader = {
	.name = "CBFS",
	.load = cbfs_load_ramstage,
};
