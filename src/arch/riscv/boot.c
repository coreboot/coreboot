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
 */

#include <program_loading.h>
#include <vm.h>
#include <arch/boot.h>
#include <arch/encoding.h>
#include <arch/smp/smp.h>
#include <mcall.h>
#include <commonlib/cbfs_serialized.h>

/*
 * A pointer to the Flattened Device Tree passed to coreboot by the boot ROM.
 * Presumably this FDT is also in ROM.
 *
 * This pointer is only used in ramstage!
 */

static void do_arch_prog_run(struct prog *prog)
{
	void (*doit)(int hart_id, void *fdt);
	int hart_id;
	void *fdt = prog_entry_arg(prog);

	/*
	 * Workaround selfboot putting the coreboot table into prog_entry_arg
	 */
	if (prog_cbfs_type(prog) == CBFS_TYPE_SELF)
		fdt = HLS()->fdt;

	/*
	 * If prog_entry_arg is not set (e.g. by fit_payload), use fdt from HLS
	 * instead.
	 */
	if (fdt == NULL)
		fdt = HLS()->fdt;

	if (ENV_RAMSTAGE && prog_type(prog) == PROG_PAYLOAD) {
		run_payload(prog, fdt, RISCV_PAYLOAD_MODE_S);
		return;
	}

	doit = prog_entry(prog);
	hart_id = HLS()->hart_id;

	doit(hart_id, fdt);
}

void arch_prog_run(struct prog *prog)
{
	smp_resume((void (*)(void *))do_arch_prog_run, prog);
}
