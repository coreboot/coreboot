/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <program_loading.h>

void main(void)
{
	console_init();
	run_ramstage();
}
