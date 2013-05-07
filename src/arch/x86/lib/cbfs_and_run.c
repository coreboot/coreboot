/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
#include <timestamp.h>

static void cbfs_and_run_core(const char *filename)
{
	u8 *dst;

	timestamp_add_now(TS_START_COPYRAM);
	print_debug("Loading image.\n");
	dst = cbfs_load_stage(CBFS_DEFAULT_MEDIA, filename);
	if ((void *)dst == (void *) -1)
		die("FATAL: Essential component is missing.\n");

	timestamp_add_now(TS_END_COPYRAM);
	print_debug("Jumping to image.\n");
	__asm__ volatile (
		"jmp  *%%edi\n"
		:: "D"(dst)
	);
}

void asmlinkage copy_and_run(void)
{
	cbfs_and_run_core(CONFIG_CBFS_PREFIX "/coreboot_ram");
}
