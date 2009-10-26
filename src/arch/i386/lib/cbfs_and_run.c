/*
 * This file is part of the coreboot project.
 * 
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <cbfs.h>

void cbfs_and_run_core(char *filename, unsigned ebp)
{
	u8 *dst;

	print_debug("Loading stage image.\n");
	dst = cbfs_load_stage(filename);
	if (dst == (void *) -1) {
		/* We should use die() here. */
		print_emerg("Loading stage failed!\n");
		for (;;)
			asm("hlt\n");
	}

	print_debug("Jumping to image.\n");
	__asm__ volatile (
		"movl %%eax, %%ebp\n"
		"jmp  *%%edi\n"
		:: "a"(ebp), "D"(dst)
	);
}
