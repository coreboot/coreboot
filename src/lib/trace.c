/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <trace.h>

int volatile trace_dis = 0;

void __cyg_profile_func_enter(void *func, void *callsite)
{

	if (trace_dis)
		return;

	DISABLE_TRACE
	printk(BIOS_INFO, "~%p(%p)\n", func, callsite);
	ENABLE_TRACE
}

void __cyg_profile_func_exit(void *func, void *callsite)
{
}
