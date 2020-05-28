/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl_uapp/bl_syscall_public.h>
#include <console/console.h>
#include <reset.h>
#include <stdint.h>


void do_board_reset(void)
{
	printk(BIOS_ERR, "Resetting the board now.\n");
	svc_reset_system(RESET_TYPE_COLD);
}
