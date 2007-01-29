/* GPLv2.
 *
 * Copyright (C) 2007 Stefan Reinauer <stepan@coresystems.de>, coresystems GmbH
 */

#include <arch/types.h>
#include <arch/io.h>
#include <console/loglevel.h>

static void post_code(u8 value)
{
	outb(value, 0x80);
}

int main(void)
{
	printk(BIOS_INFO, "RAM init code started\n");

	die ("Nothing to do.");

	return 0;
}


