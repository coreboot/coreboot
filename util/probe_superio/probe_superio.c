/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2006 Ronald Minnich <rminnich@gmail.com>
 * Copyright (C) 2006 coresystems GmbH <info@coresystems.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>

/* well, they really thought this through, eh? Family is 8 bits!!!! */
char *familyid[] = {
	[0xf1] = "pc8374 (winbond, was NS)"
};

/* eventually, if you care, break this out into a file. For now, I don't know 
  * if we need this. 
  */

unsigned char regval(unsigned short port, unsigned short reg) {
	outb(reg, port);
	return inb(port+1);
}

void
dump_ns8374(unsigned short port) {
	printf("Enables: 21=%02x, 22=%02x, 23=%02x, 24=%02x, 26=%02x\n", 
			regval(port,0x21), regval(port,0x22), 
			regval(port,0x23), regval(port,0x24), regval(port,0x26));
	printf("SMBUS at %02x\n", regval(port, 0x2a));
	/* check COM1. This is all we care about at present. */
	printf("COM 1 is Globally %s\n", regval(port,0x26)&8 ? "disabled" : "enabled");
	/* select com1 */
	outb(0x7, port);
	outb(3, port+1);
	printf("COM 1 is locally %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("COM1 60=%02x, 61=%02x, 70=%02x, 71=%02x, 74=%02x, 75=%02x, f0=%02x\n", 
		regval(port, 0x60), regval(port, 0x61), regval(port, 0x70), regval(port, 0x71),
		regval(port, 0x74), regval(port, 0x75), regval(port, 0xf0));
	/* select gpio */
	outb(0x7, port);
	outb(7, port+1);
	printf("GPIO is %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("GPIO 60=%02x, 61=%02x, 70=%02x, 71=%02x, 74=%02x, 75=%02x, f0=%02x\n", 
		regval(port, 0x60), regval(port, 0x61), regval(port, 0x70), regval(port, 0x71),
		regval(port, 0x74), regval(port, 0x75), regval(port, 0xf0));

}

void
dump_fintek(unsigned short port, unsigned int did) 
{
	switch(did) {
	case 0x0604:
		printf ("Fintek F71805\n");
		break;
	case 0x4103:
		printf ("Fintek F71872\n");
		break;
	default:
		printf ("Unknown Fintek SuperIO: did=0x%04x\n",did);
		return;
	}

	printf("Flash write is %s.\n", regval(port, 0x28)&0x80 ? "enabled" : "disabled");
	printf("Flash control is 0x%04x.\n", regval(port, 0x28));
	printf("27=%02x\n", regval(port, 0x27));
	printf("29=%02x\n", regval(port, 0x29));
	printf("2a=%02x\n", regval(port, 0x2a));
	printf("2b=%02x\n", regval(port, 0x2b));

	/* select UART 1 */
	outb(0x07, port);
	outb(0x01, port+1);
	printf("UART1 is %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("UART1 base=%02x%02x, irq=%02x, mode=%s\n", 
		regval(port, 0x60), regval(port, 0x61), regval(port, 0x70)&0x0f, 
		regval(port, 0xf0)&0x10 ? "RS485":"RS232");

	/* select UART 2 */
	outb(0x07, port);
	outb(0x02, port+1);
	printf("UART2 is %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("UART2 base=%02x%02x, irq=%02x, mode=%s\n", 
		regval(port, 0x60), regval(port, 0x61), regval(port, 0x70)&0x0f, 
		regval(port, 0xf0)&0x10 ? "RS485":"RS232");

	/* select Parport */
	outb(0x07, port);
	outb(0x03, port+1);
	printf("PARPORT is %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("PARPORT base=%02x%02x, irq=%02x\n", 
		regval(port, 0x60), regval(port, 0x61), regval(port, 0x70)&0x0f);

	/* select hw monitor */
	outb(0x07, port);
	outb(0x04, port+1);
	printf("HW monitor is %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("HW monitor base=%02x%02x, irq=%02x\n", 
		regval(port, 0x60), regval(port, 0x61), regval(port, 0x70)&0x0f);

	/* select gpio */
	outb(0x07, port);
	outb(0x06, port+1);
	printf("GPIO is %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("GPIO 70=%02x, e0=%02x, e1=%02x, e2=%02x, e3=%02x, e4=%02x, e5=%02x\n", 
		regval(port, 0x70), regval(port, 0xe0), regval(port, 0xe1), regval(port, 0xe2),
		regval(port, 0xe3), regval(port, 0xe4), regval(port, 0xe5));
	printf("GPIO e6=%02x, e7=%02x, e8=%02x, e9=%02x, f0=%02x, f1=%02x, f3=%02x, f4=%02x\n", 
		regval(port, 0xe6), regval(port, 0xe7), regval(port, 0xe8), regval(port, 0xe9),
		regval(port, 0xf0), regval(port, 0xf1), regval(port, 0xf3), regval(port, 0xf4));
	printf("GPIO f5=%02x, f6=%02x, f7=%02x, f8=%02x\n", 
		regval(port, 0xf5), regval(port, 0xf6), regval(port, 0xf7), regval(port, 0xf8));


}


void
probe_idregs_simple(unsigned short port){
	unsigned char id;
	outb(0x20, port);
	if (inb(port) != 0x20)  {
		if (inb(port) == 0xff )
			printf ("No SuperIO chip found at 0x%04x\n", port);
		else
			printf("probing 0x%04x, failed (0x%02x), data returns 0x%02x\n", 
					port, inb(port), inb(port+1));
		return;
	}
	id = inb(port+1);

	printf("SuperIO found at 0x%02x: id = 0x%02x\n", port, id);
	if (id == 0xff)
		return;

	if (familyid[id]) 
		printf("%s\n", familyid[id]);
	else
		printf("<unknown>\n");

	switch(id) {
		case 0xf1:
			dump_ns8374(port);
			break;
		default:
			printf("no dump for 0x%02x\n", id);
			break;
	}
}


void
probe_idregs_fintek(unsigned short port){
	unsigned int vid, did;

	// Enable configuration sequence (Fintek uses this for example)
	outb(0x87, port);
	outb(0x87, port);

	// 
	outb(0x20, port);
	if (inb(port) != 0x20)  {
		if (inb(port) == 0xff )
			printf ("No SuperIO chip found at 0x%04x\n", port);
		else
			printf("probing 0x%04x, failed (0x%02x), data returns 0x%02x\n", 
					port, inb(port), inb(port+1));
		return;
	}
	did = inb(port+1);

	outb(0x21, port);
	did = did|(inb(port+1)<<8);

	outb(0x23, port);
	vid = inb(port+1);
	outb(0x24, port);
	vid = vid|(inb(port+1)<<8);

	printf("SuperIO found at 0x%02x: vid=0x%04x/did=0x%04x\n", port, vid, did);

	if (vid == 0xff || vid == 0xffff)
		return;

	// printf("%s\n", familyid[id]);
	switch(vid) {
		case 0x3419:
			dump_fintek(port, did);
			break;
		default:
			printf("no dump for 0x%04x/0x%04x\n", vid, did);
			break;
	}

	// disable configuration
	outb(0xaa, port);
}

void
probe_superio(unsigned short port) {
	probe_idregs_simple(port);
	probe_idregs_fintek(port);
}

int
main(int argc, char *argv[])
{
	if (iopl(3) < 0) {
		perror("iopl");
		exit(1);
	}

	/* try the 2e */
	probe_superio(0x2e);
	/* now try the 4e */
	probe_superio(0x4e);

	return 0;
}
