/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <post.h>
#include <stdint.h>

void arch_post_code(uint8_t value)
{
	if (CONFIG(POST_IO))
		outb(value, CONFIG_POST_IO_PORT);

	if (CONFIG(CMOS_POST) && !ENV_SMM)
		cmos_post_code(value);
}
