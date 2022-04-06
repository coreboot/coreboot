/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>
#include <romstage_common.h>

void main(void)
{
	console_init();
	romstage_main();
}

void __noreturn romstage_main(void)
{
	cbmem_initialize_empty();
	run_ramstage();
}
