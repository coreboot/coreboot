/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/exception.h>
#include <arch/stages.h>
#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>
#include <romstage_common.h>
#include <timestamp.h>

__weak void platform_romstage_main(void) { /* no-op, for bring-up */ }
__weak void platform_romstage_postram(void) { /* no-op */ }

void main(void)
{
	timestamp_add_now(TS_ROMSTAGE_START);

	console_init();

	exception_init();
	romstage_main();
}

void __noreturn romstage_main(void)
{
	platform_romstage_main();
	cbmem_initialize_empty();
	platform_romstage_postram();

	run_ramstage();
}
