/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Carl-Daniel Hailfinger
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console.h>

int printktest(void)
{
	/* If printk succeeds, it will print the message below. This is not a
	 * success message after a test, but a success message used as test.
	 * In case of compiler/linker bugs the printk call is likely to crash.
	 */
	printk(BIOS_INFO, "Absolute call successful.\n");

	return 0;
}
