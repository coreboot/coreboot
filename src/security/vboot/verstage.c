/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/exception.h>
#include <arch/hlt.h>
#include <console/console.h>
#include <program_loading.h>
#include <security/vboot/vboot_common.h>

void __weak verstage_mainboard_init(void)
{
	/* Default empty implementation. */
}

void main(void)
{
	console_init();
	exception_init();
	verstage_mainboard_init();

	if (CONFIG(VBOOT_RETURN_FROM_VERSTAGE)) {
		verstage_main();
		printk(BIOS_DEBUG, "VBOOT: Returning from verstage.\n");
	} else {
		run_romstage();
		hlt();
	}
}
