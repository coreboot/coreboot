/*
 * Copyright (C) 2013 The ChromeOS Authors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <console/console.h>

void main(void)
{
//	volatile unsigned long *pshold = (unsigned long *)0x1004330c;
//	*pshold &= ~0x100;	/* shut down */

	/* FIXME: console_init() seems to cause things to die... Maybe
	   we need to reset our stack pointer? */
//	console_init();
	printk(BIOS_INFO, "hello from ramstage\n");
	while (1);
}
