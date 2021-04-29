/* SPDX-License-Identifier: GPL-2.0-only */

/* This file contains stub for not-yet-implemented svc in cezanne PSP.
 * So this file will and should be removed eventually when psp_verstage works
 * correctly in cezanne.
 */

#include <bl_uapp/bl_syscall_public.h>
#include <console/console.h>
#include <psp_verstage.h>
#include <reset.h>
#include <timer.h>

uint32_t svc_write_postcode(uint32_t postcode)
{
	return 0;
}

static uint64_t tmp_timer_value = 0;
void timer_monotonic_get(struct mono_time *mt)
{
	mt->microseconds = tmp_timer_value / 1000;
	tmp_timer_value++;
}

void do_board_reset(void)
{
	printk(BIOS_ERR, "Reset not implemented yet.\n");
	while (1)
		;
}
