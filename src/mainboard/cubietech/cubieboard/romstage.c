/*
 * Placeholder for Cubieboard romstage
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include <console/console.h>

void main(void)
{
	console_init();
	printk(BIOS_INFO, "You have managed to succesfully load romstage.\n");
}
