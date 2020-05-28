/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl_uapp/bl_syscall_public.h>
#include <delay.h>
#include <stdint.h>

void udelay(uint32_t usecs)
{
	svc_delay_in_usec(usecs);
}
