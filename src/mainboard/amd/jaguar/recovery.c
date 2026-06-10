/* SPDX-License-Identifier: GPL-2.0-only */
#include <amdblocks/psp.h>
#include <cf9_reset.h>
#include <console/console.h>

void die_notify(void)
{
	printk(BIOS_EMERG, "\nToggling boot partition and resetting system...\n");
	psp_ab_recovery_toggle_bootpartition();
	system_reset();
}
