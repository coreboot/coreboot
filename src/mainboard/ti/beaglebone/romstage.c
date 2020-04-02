/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <program_loading.h>
#include <console/console.h>

void main(void)
{
	console_init();
	printk(BIOS_INFO, "Hello from romstage.\n");

	run_ramstage();
}
