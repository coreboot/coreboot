/*******************************************************************************
 *
 *
 *	Copyright 2003 Steven James <pyro@linuxlabs.com> and
 *	LinuxLabs http://www.linuxlabs.com
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ******************************************************************************/

#include "uhci.h"
#include "debug.h"
#include <arch/io.h>
#include <rom/read_bytes.h>

struct lb_memory *lbmem;
extern int usec_offset;

extern struct stream usb_stream;

int main(void)
{
	unsigned char value;

	printk("LinuxLabs USB bootloader\n");

	outb( 0x30, 0x70);	// reset primary boot
	outb( 0xff, 0x71);
	lbmem = get_lbmem();

	init_devices();
	uhci_init();

	usb_stream.init();
	elfboot(&usb_stream, lbmem);

	while(1) {
		poll_usb();
//		usec_offset+=10;
//		printk("USEC_OFFSET = %d\n", usec_offset);
	}

	return(0);
}
