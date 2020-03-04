/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#include <console/console.h>

void __div0(void); // called from asm so no need for a prototype in a header

/* Replacement (=dummy) for GNU/Linux division-by zero handler */
/* recursion is ok here because we have no formats ... */
void __div0 (void)
{
	printk(BIOS_EMERG, "DIVIDE BY ZERO! continuing ...\n");
}
