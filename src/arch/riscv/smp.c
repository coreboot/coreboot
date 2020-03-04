/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stddef.h>
#include <arch/encoding.h>
#include <arch/smp/smp.h>
#include <arch/smp/spinlock.h>
#include <mcall.h>
#include <console/console.h>

void smp_pause(int working_hartid)
{
#define SYNCA (OTHER_HLS(working_hartid)->entry.sync_a)
#define SYNCB (OTHER_HLS(working_hartid)->entry.sync_b)

	int hartid = read_csr(mhartid);

	if (hartid != working_hartid) {
		/* waiting for work hart */
		do {
			barrier();
		} while (atomic_read(&SYNCA) != 0x01234567);

		clear_csr(mstatus, MSTATUS_MIE);
		write_csr(mie, MIP_MSIP);

		/* count how many cores enter the halt */
		atomic_add(&SYNCB, 1);

		do {
			barrier();
			__asm__ volatile ("wfi");
		} while ((read_csr(mip) & MIP_MSIP) == 0);
		set_msip(hartid, 0);
		HLS()->entry.fn(HLS()->entry.arg);
	} else {
		/* Initialize the counter and
		 * mark the work hart into smp_pause */
		atomic_set(&SYNCB, 0);
		atomic_set(&SYNCA, 0x01234567);

		/* waiting for other Hart to enter the halt */
		do {
			barrier();
		} while (atomic_read(&SYNCB) + 1 < CONFIG_MAX_CPUS);

		/* initialize for the next call */
		atomic_set(&SYNCA, 0);
		atomic_set(&SYNCB, 0);
	}
#undef SYNCA
#undef SYNCB
}

void smp_resume(void (*fn)(void *), void *arg)
{
	int hartid = read_csr(mhartid);

	if (fn == NULL)
		die("must pass a non-null function pointer\n");

	for (int i = 0; i < CONFIG_MAX_CPUS; i++) {
		OTHER_HLS(i)->entry.fn = fn;
		OTHER_HLS(i)->entry.arg = arg;
	}

	for (int i = 0; i < CONFIG_MAX_CPUS; i++)
		if (i != hartid)
			set_msip(i, 1);

	if (HLS()->entry.fn == NULL)
		die("entry fn not set\n");

	HLS()->entry.fn(HLS()->entry.arg);
}
