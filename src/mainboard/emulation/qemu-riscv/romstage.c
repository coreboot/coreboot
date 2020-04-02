/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>

void main(void)
{
	console_init();
	cbmem_initialize_empty();
	run_ramstage();
}
