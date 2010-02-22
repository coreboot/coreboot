/*
 * This file is part of the coreboot project.
 * 
 * Copyright (C) 2009-2010 coresystems GmbH
 * Written by Patrick Georgi <patrick.georgi@coresystems.de> 
 * for coresystems GmbH
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

void cbfs_and_run_core(const char *filename, unsigned ebp);

static void copy_and_run(unsigned cpu_reset)
{
	if (cpu_reset == 1) cpu_reset = -1;
	else cpu_reset = 0;

	cbfs_and_run_core(CONFIG_CBFS_PREFIX "/coreboot_ram", cpu_reset);
}
