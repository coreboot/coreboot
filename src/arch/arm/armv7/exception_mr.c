/* SPDX-License-Identifier: BSD-3-Clause */
/* This file is part of the coreboot project. */

#include <arch/exception.h>
#include <console/console.h>

void exception_init(void)
{
	printk(BIOS_DEBUG, "Exception handlers not installed.\n");
}
