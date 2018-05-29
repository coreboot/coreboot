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
 * This file contains entry/exit functions for each stage during coreboot
 * execution (bootblock entry and ramstage exit will depend on external
 * loading).
 *
 * Entry points must be placed at the location the previous stage jumps
 * to (the lowest address in the stage image). This is done by giving
 * stage_entry() its own section in .text and placing it first in the
 * linker script.
 */

#include <arch/boot.h>
#include <arch/encoding.h>
#include <arch/stages.h>
#include <rules.h>

void stage_entry(void)
{
	/*
	 * Save the FDT pointer before entering ramstage, because mscratch
	 * might be overwritten in the trap handler, and there is code in
	 * ramstage that generates misaligned access faults.
	 */
	if (ENV_RAMSTAGE)
		rom_fdt = (const void *)read_csr(mscratch);

	main();
}
