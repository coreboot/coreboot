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

#include <arch/exception.h>
#include <arch/hlt.h>
#include <bootblock_common.h>
#include <cbfs.h>
#include <console/console.h>
#include <arch/stages.h>

// the qemu part of all this is very, very non-hardware like.
// so it gets its own bootblock.
void main(void)
{
	void *entry;

	if (IS_ENABLED(CONFIG_BOOTBLOCK_CONSOLE)) {
		console_init();
		exception_init();
	}

	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA, CONFIG_CBFS_PREFIX"/romstage");
	if (! entry) {
		printk(BIOS_EMERG, "AAAAAAAAAAAAAA no romstage!\n");
		while (1);
	}

	stage_exit(entry);
}
