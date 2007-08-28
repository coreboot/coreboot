/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2006 Ronald Minnich <rminnich@gmail.com>
 * Copyright (C) 2006 coresystems GmbH <info@coresystems.de>
 * Copyright (C) 2007 Carl-Daniel Hailfinger
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

unsigned char regval(unsigned short port, unsigned char reg) {
	outb(reg, port);
	return inb(port + 1);
}

void regwrite(unsigned short port, unsigned char reg, unsigned char val) {
	outb(reg, port);
	outb(val, port + 1);
}

void
dump_ns8374(unsigned short port) {
	printf("Enables: 21=%02x, 22=%02x, 23=%02x, 24=%02x, 26=%02x\n",
		regval(port, 0x21), regval(port, 0x22), regval(port, 0x23),
		regval(port, 0x24), regval(port, 0x26));
	printf("SMBUS at %02x\n", regval(port, 0x2a));
	/* check COM1. This is all we care about at present. */
	printf("COM 1 is Globally %s\n", regval(port, 0x26) & 8 ? "disabled" : "enabled");
	/* select com1 */
	regwrite(port, 0x07, 0x03);
	printf("COM 1 is locally %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("COM1 60=%02x, 61=%02x, 70=%02x, 71=%02x, 74=%02x, 75=%02x, f0=%02x\n", 
		regval(port, 0x60), regval(port, 0x61), regval(port, 0x70), regval(port, 0x71),
		regval(port, 0x74), regval(port, 0x75), regval(port, 0xf0));
	/* select gpio */
	regwrite(port, 0x07, 0x07);
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
		printf ("Unknown Fintek SuperI/O: did=0x%04x\n", did);
		return;
	}

	printf("Flash write is %s.\n", regval(port, 0x28) & 0x80 ? "enabled" : "disabled");
	printf("Flash control is 0x%04x.\n", regval(port, 0x28));
	printf("27=%02x\n", regval(port, 0x27));
	printf("29=%02x\n", regval(port, 0x29));
	printf("2a=%02x\n", regval(port, 0x2a));
	printf("2b=%02x\n", regval(port, 0x2b));

	/* select UART 1 */
	regwrite(port, 0x07, 0x01);
	printf("UART1 is %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("UART1 base=%02x%02x, irq=%02x, mode=%s\n", 
		regval(port, 0x60), regval(port, 0x61), regval(port, 0x70) & 0x0f, 
		regval(port, 0xf0) & 0x10 ? "RS485":"RS232");

	/* select UART 2 */
	regwrite(port, 0x07, 0x02);
	printf("UART2 is %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("UART2 base=%02x%02x, irq=%02x, mode=%s\n", 
		regval(port, 0x60), regval(port, 0x61), regval(port, 0x70) & 0x0f, 
		regval(port, 0xf0) & 0x10 ? "RS485":"RS232");

	/* select Parport */
	regwrite(port, 0x07, 0x03);
	printf("PARPORT is %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("PARPORT base=%02x%02x, irq=%02x\n", 
		regval(port, 0x60), regval(port, 0x61), regval(port, 0x70) & 0x0f);

	/* select hw monitor */
	regwrite(port, 0x07, 0x04);
	printf("HW monitor is %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("HW monitor base=%02x%02x, irq=%02x\n", 
		regval(port, 0x60), regval(port, 0x61), regval(port, 0x70) & 0x0f);

	/* select gpio */
	regwrite(port, 0x07, 0x05);
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

/* End Of Table */
#define EOT -1
/* NO LDN needed */
#define NOLDN -2
/* Not Available */
#define NANA -3
/* Maximum Name Length */
#define MAXNAMELEN 20
/* Biggest LDN */
#define MAXLDN 0xa
/* biggestLDN + 0 + NOLDN + EOT */
#define LDNSIZE MAXLDN + 3
/* MAXimum NUMber of Indexes */
#define MAXNUMIDX 70
#define IDXSIZE MAXNUMIDX + 1

const static struct ite_registers {
	/* yes, superio_id should be unsigned, but EOT has to be negative */
	signed short superio_id;
	char name[MAXNAMELEN];
	struct ite_ldnidx {
		signed short ldn;
		signed short idx[IDXSIZE];
		signed short def[IDXSIZE];
	} ldn[LDNSIZE];
} ite_reg_table[] = {
	{0x8702, "IT8702", {
		{EOT}}},
	{0x8705, "IT8705 or IT8700", {
		{EOT}}},
	{0x8710, "IT8710", {
		{EOT}}},
	{0x8712, "IT8712", {
		{NOLDN,
			{0x07,0x20,0x21,0x22,0x23,0x24,0x2b,EOT},
			{NANA,0x87,0x12,0x08,0x00,0x00,0x00,EOT}},
		{0x0,
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,EOT},
			{0x00,0x03,0xf0,0x06,0x02,0x00,0x00,EOT}},
		{0x1,
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,0xf3,EOT},
			{0x00,0x03,0xf8,0x04,0x00,0x50,0x00,0x7f,EOT}},
		{0x2,
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,0xf3,EOT},
			{0x00,0x02,0xf8,0x03,0x00,0x50,0x00,0x7f,EOT}},
		{0x3,
			{0x30,0x60,0x61,0x62,0x63,0x70,0x74,0xf0,EOT},
			{0x00,0x03,0x78,0x07,0x78,0x07,0x03,0x03,EOT}},
		{0x4,
			{0x30,0x60,0x61,0x62,0x63,0x70,0xf0,0xf1,0xf2,0xf3,
			 0xf4,0xf5,0xf6,EOT},
			{0x00,0x02,0x90,0x02,0x30,0x09,0x00,0x00,0x00,0x00,
			 0x00,NANA,NANA,EOT}},
		{0x5,
			{0x30,0x60,0x61,0x62,0x63,0x70,0x71,0xf0,EOT},
			{0x01,0x00,0x60,0x00,0x64,0x01,0x02,0x00,EOT}},
		{0x6,
			{0x30,0x70,0x71,0xf0,EOT},
			{0x00,0x0c,0x02,0x00,EOT}},
		{0x7,
			{0x25,0x26,0x27,0x28,0x29,0x2a,0x2c,0x60,0x61,0x62,
			 0x63,0x64,0x65,0x70,0x71,0x72,0x73,0x74,0xb0,0xb1,
			 0xb2,0xb3,0xb4,0xb5,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,
			 0xc0,0xc1,0xc2,0xc3,0xc4,0xc8,0xc9,0xca,0xcb,0xcc,
			 0xe0,0xe1,0xe2,0xe3,0xe4,0xf0,0xf1,0xf2,0xf3,0xf4,
			 0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,EOT},
			{0x01,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,
			 0x00,0x00,0x00,0x00,0x00,0x30,0x38,0x00,0x00,0x00,
			 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			 0x01,0x00,0x00,0x40,0x00,0x01,0x00,0x00,0x40,0x00,
			 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			 0x00,0x00,0x00,0x00,0x00,0x00,0x00,NANA,0x00,EOT}},
		{0x8,
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x00,0x03,0x00,0x0a,0x00,EOT}},
		{0x9,
			{0x30,0x60,0x61,EOT},
			{0x00,0x02,0x01,EOT}},
		{0xa,
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x00,0x03,0x10,0x0b,0x00,EOT}},
		{EOT}}},
	{0x8716, "IT8716", {
		{EOT}}},
	{0x8718, "IT8718", {
		{EOT}}},
	{EOT}
};

void
dump_ite(unsigned short port, unsigned short id) 
{
	int i, j, k;
	signed short *idx;
	printf ("ITE ");


	/* ID Mapping Table
	   unknown -> IT8711 (no datasheet)
	   unknown -> IT8722 (no datasheet)
	   0x8702 -> IT8702
	   0x8705 -> IT8700 or IT8705
	   0x8708 -> IT8708
	   0x8710 -> IT8710
	   0x8712 -> IT8712
	   0x8716 -> IT8716
	   0x8718 -> IT8718
	   0x8726 -> IT8726 (datasheet wrongly says 0x8716)
	*/
	switch(id) {
	case 0x8702:
	case 0x8705:
	case 0x8710:
	case 0x8712:
	case 0x8716:
	case 0x8718:
		for (i=0;; i++) {
			if (ite_reg_table[i].superio_id == EOT)
				break;
			if ((unsigned short)ite_reg_table[i].superio_id != id)
				continue;
			printf ("%s\n", ite_reg_table[i].name);
			for (j=0;; j++) {
				if (ite_reg_table[i].ldn[j].ldn == EOT)
					break;
				if (ite_reg_table[i].ldn[j].ldn != NOLDN) {
					printf("switching to LDN 0x%01x\n",
					       ite_reg_table[i].ldn[j].ldn);
					regwrite(port, 0x07,
					         ite_reg_table[i].ldn[j].ldn);
				}
				idx = ite_reg_table[i].ldn[j].idx;
				printf("idx ");
				for (k=0;; k++) {
					if (idx[k] == EOT)
						break;
					printf("%02x ", idx[k]);
				}
				printf("\nval ");
				for (k=0;; k++) {
					if (idx[k] == EOT)
						break;
					printf("%02x ", regval(port, idx[k]));
				}
				printf("\ndef ");
				idx = ite_reg_table[i].ldn[j].def;
				for (k=0;; k++) {
					if (idx[k] == EOT)
						break;
					if (idx[k] == NANA)
						printf("NA ");
					else
						printf("%02x ", idx[k]);
				}
				printf("\n");
			}
					
		}
		break;
	default:
		printf ("unknown ITE chip, id=%04x\n", id);
		for (i=0x20; i<=0x24; i++)
			printf("index %02x=%02x\n", i, regval(port, i));
		break;
	}
}

void
probe_idregs_simple(unsigned short port){
	unsigned char id;
	outb(0x20, port);
	if (inb(port) != 0x20)  {
		if (inb(port) == 0xff )
			printf ("No SuperI/O chip found at 0x%04x\n", port);
		else
			printf("probing 0x%04x, failed (0x%02x), data returns 0x%02x\n", 
					port, inb(port), inb(port + 1));
		return;
	}
	id = inb(port + 1);

	printf("SuperI/O found at 0x%02x: id = 0x%02x\n", port, id);
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
	unsigned int vid, did, success = 0;

	/* Enable configuration sequence (Fintek uses this for example)
	   Older ITE chips have the same enable sequence */
	outb(0x87, port);
	outb(0x87, port);

	outb(0x20, port);
	if (inb(port) != 0x20)  {
		if (inb(port) == 0xff )
			printf ("No SuperIO chip found at 0x%04x\n", port);
		else
			printf("probing 0x%04x, failed (0x%02x), data returns 0x%02x\n", 
					port, inb(port), inb(port + 1));
		return;
	}
	did = inb(port + 1);

	did |= (regval(port, 0x21)<<8);

	vid = regval(port, 0x23);
	vid |= (regval(port, 0x24)<<8);

	printf("SuperIO found at 0x%02x: vid=0x%04x/did=0x%04x\n", port, vid, did);

	if (vid == 0xff || vid == 0xffff)
		return;

	/* printf("%s\n", familyid[id]); */
	switch(did) {
		case 0x0887: /* pseudoreversed for ITE8708 */
		case 0x1087: /* pseudoreversed for ITE8710 */
			success = 1;
			dump_ite(port, ((did & 0xff) << 8) | ((did & 0xff00) >> 8));
			/* disable configuration */
			regwrite(port, 0x02, 0x02);
			break;
		default:
			break;
	}
	switch(vid) {
		case 0x3419:
			success = 1;
			dump_fintek(port, did);
			break;
		default:
			break;
	}
	if (!success)
		printf("no dump for vid 0x%04x, did 0x%04x\n", vid, did);

	/* disable configuration (for Fintek, doesn't hurt ITE) */
	outb(0xaa, port);
}

void
probe_idregs_ite(unsigned short port){
	unsigned int id, chipver;

	/* Enable configuration sequence (ITE uses this for newer IT87[012]x)
	   IT871[01] uses 0x87, 0x87 -> fintek detection should handle it
	   IT8761 uses 0x87, 0x61, 0x55, 0x55/0xaa
	   IT86xx series uses different ports
	   IT8661 uses 0x86, 0x61, 0x55/0xaa, 0x55/0xaa and 32 more writes
	   IT8673 uses 0x86, 0x80, 0x55/0xaa, 0x55/0xaa and 32 more writes */
	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	if (port == 0x2e)
		outb(0x55, port);
	else
		outb(0xAA, port);

	/* Read Chip ID Byte 1 */
	id = regval(port, 0x20);
	if (id != 0x87)  {
		if (inb(port) == 0xff )
			printf ("No SuperIO chip found at 0x%04x\n", port);
		else
			printf("probing 0x%04x, failed (0x%02x), data returns 0x%02x\n", 
					port, inb(port), inb(port + 1));
		return;
	}

	id <<= 8;

	/* Read Chip ID Byte 2 */
	id |= regval(port, 0x21);

	/* Read Chip Version, only bit 3..0 for all IT87xx */
	chipver = regval(port, 0x22) & 0x0f;

	/* ID Mapping Table
	   unknown -> IT8711 (no datasheet)
	   unknown -> IT8722 (no datasheet)
	   0x8702 -> IT8702
	   0x8705 -> IT8700 or IT8705
	   0x8710 -> IT8710
	   0x8712 -> IT8712
	   0x8716 -> IT8716
	   0x8718 -> IT8718
	   0x8726 -> IT8726 (datasheet wrongly says 0x8716)
	*/
	printf("SuperI/O found at 0x%02x: id=0x%04x, chipver=0x%01x\n",
	       port, id, chipver);

	switch(id) {
		case 0x8702:
		case 0x8705:
		case 0x8712:
		case 0x8716:
		case 0x8718:
		case 0x8726:
			dump_ite(port, id);
			break;
		default:
			printf("no dump for id 0x%04x\n", id);
			break;
	}
	/* disable configuration */
	regwrite(port, 0x02, 0x02);
}

void
probe_superio(unsigned short port) {
	probe_idregs_simple(port);
	probe_idregs_fintek(port);
	probe_idregs_ite(port);
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
