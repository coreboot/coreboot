/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>
#include <program_loading.h>

void main(void)
{
	console_init();
	run_ramstage();
}
