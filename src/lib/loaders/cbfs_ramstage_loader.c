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
#include <program_loading.h>

#if CONFIG_RELOCATABLE_RAMSTAGE
#include <rmodule.h>
#include <cbmem.h>

static int cbfs_load_ramstage(struct prog *ramstage)
{
	struct rmod_stage_load rmod_ram = {
		.cbmem_id = CBMEM_ID_RAMSTAGE,
		.prog = ramstage,
	};

	if (rmodule_stage_load_from_cbfs(&rmod_ram)) {
		printk(BIOS_DEBUG, "Could not load ramstage.\n");
		return -1;
	}

	return 0;
}

#else /* CONFIG_RELOCATABLE_RAMSTAGE */

static int cbfs_load_ramstage(struct prog *ramstage)
{
	return cbfs_load_prog_stage(CBFS_DEFAULT_MEDIA, ramstage);

}

#endif /* CONFIG_RELOCATABLE_RAMSTAGE */

const struct prog_loader_ops cbfs_ramstage_loader = {
	.name = "CBFS",
	.prepare = cbfs_load_ramstage,
};
