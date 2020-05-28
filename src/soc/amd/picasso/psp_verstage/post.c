/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl_uapp/bl_syscall_public.h>
#include <console/console.h>
#include <post.h>

void arch_post_code(u8 value)
{
	if (CONFIG(POST_IO) && CONFIG_POST_IO_PORT == 0x80)
		svc_write_postcode(value);
}
