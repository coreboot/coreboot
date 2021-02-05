/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Entry points must be placed at the location the previous stage jumps
 * to (the lowest address in the stage image). This is done by giving
 * stage_entry() its own section in .text and placing it first in the
 * linker script.
 */

#include <arch/stages.h>
#include <arch/smp/smp.h>
#include <mcall.h>

void stage_entry(int hart_id, void *fdt)
{
	HLS()->hart_id = hart_id;
	HLS()->fdt = fdt;
	smp_pause(CONFIG_RISCV_WORKING_HARTID);

	main();
}
