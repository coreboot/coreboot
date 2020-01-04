/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <post.h>
#include <stdint.h>
#include <console/console.h>
#include <arch/io.h>

void arch_post_code(uint8_t value)
{
	if (CONFIG(POST_IO))
		outb(value, CONFIG_POST_IO_PORT);

	if (CONFIG(CMOS_POST) && !ENV_SMM)
		cmos_post_code(value);
}
