/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Entry points must be placed at the location the previous stage jumps
 * to (the lowest address in the stage image). This is done by giving
 * stage_entry() its own section in .text and placing it first in the
 * linker script.
 */

#include <arch/stages.h>
#include <arch/smp/smp.h>
#include <rules.h>
#include <mcall.h>

void stage_entry(int hart_id, void *fdt)
{
	HLS()->hart_id = hart_id;
	HLS()->fdt = fdt;
	smp_pause(CONFIG_RISCV_WORKING_HARTID);

	main();
}
