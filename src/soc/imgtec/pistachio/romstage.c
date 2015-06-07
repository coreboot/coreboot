/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
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
 * Foundation, Inc.
 */

#include <cbmem.h>
#include <console/console.h>
#include <halt.h>
#include <program_loading.h>
#include <soc/ddr_init.h>

void main(void)
{
	int error;
	console_init();
	error = init_ddr2();

	if (!error) {
		/*
		 * When romstage is running it's always on the reboot path and
		 * never a resume path where cbmem recovery is required.
		 * Therefore, always initialize the cbmem area to be empty.
		 */
		cbmem_initialize_empty();
		run_ramstage();
	}
	halt();
}
