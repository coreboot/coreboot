/*
 * pm49fl004.c: driver for Pm49FL004 flash models.
 *
 *
 * Copyright 2004 Tyan Corporation
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
 *
 */

#include <stdio.h>
#include "flash.h"
#include "jedec.h"
#include "pm49fl004.h"

#define AUTO_PGRM		0x10
#define CHIP_ERASE		0x30
#define RESET			0xFF
#define READ_ID			0x90

static __inline__ void protect_49fl004 (volatile char * bios)
{
	/* ask compiler not to optimize this */
	volatile unsigned char tmp;

	tmp = *(volatile unsigned char *) (bios + 0x1823);
	tmp = *(volatile unsigned char *) (bios + 0x1820);
	tmp = *(volatile unsigned char *) (bios + 0x1822);
	tmp = *(volatile unsigned char *) (bios + 0x0418);
	tmp = *(volatile unsigned char *) (bios + 0x041B);
	tmp = *(volatile unsigned char *) (bios + 0x0419);
	tmp = *(volatile unsigned char *) (bios + 0x040A);
}

static __inline__ void unprotect_49fl004 (volatile char * bios)
{
	/* ask compiler not to optimize this */
	volatile unsigned char tmp;

	tmp = *(volatile unsigned char *) (bios + 0x1823);
	tmp = *(volatile unsigned char *) (bios + 0x1820);
	tmp = *(volatile unsigned char *) (bios + 0x1822);
	tmp = *(volatile unsigned char *) (bios + 0x0418);
	tmp = *(volatile unsigned char *) (bios + 0x041B);
	tmp = *(volatile unsigned char *) (bios + 0x0419);
	tmp = *(volatile unsigned char *) (bios + 0x041A);
}

static __inline__ int erase_block_49fl004 ( volatile unsigned char * bios ,unsigned long address) 
{
        volatile unsigned char *Temp;

        Temp  = bios + 0x5555; /* set up address to be C000:5555h      */
        *Temp = 0xAA;            /* write data 0xAA to the address       */
        myusec_delay(10);
        Temp  = bios + 0x2AAA; /* set up address to be C000:2AAAh      */
        *Temp = 0x55;         /* write data 0x55 to the address       */
        myusec_delay(10);
        Temp  = bios + 0x5555; /* set up address to be C000:5555h      */
        *Temp = 0x80;            /* write data 0x80 to the address       */
        myusec_delay(10);
        Temp  = bios + 0x5555; /* set up address to be C000:5555h      */
        *Temp = 0xAA;         /* write data 0xAA to the address       */
        myusec_delay(10);
        Temp  = bios + 0x2AAA; /* set up address to be C000:2AAAh      */
        *Temp = 0x55;          /* write data 0x55 to the address       */
        myusec_delay(10);
        Temp  = bios + address; /* set up address to be C000:5555h      */
        *Temp = 0x50;       /* write data 0x55 to the address       */

	/* wait for Toggle bit ready         */
        toggle_ready_jedec(bios);

        return(0);
}
static __inline__ int write_block_49fl004(volatile char * bios, 
				       unsigned char * src,
				       volatile unsigned char * dst, 
				       unsigned int page_size)
{
	int i;
	volatile char *Temp;

	for (i = 0; i < page_size; i++) {
		if (*dst != 0xff) {
			printf("FATAL: dst %p not erased (val 0x%x\n", dst, *dst);
			return(-1);
		}
		/* transfer data from source to destination */
		if (*src == 0xFF) {
			dst++, src++;
			/* If the data is 0xFF, don't program it */
			continue;
		}
		Temp =   (bios + 0x5555); 
		*Temp = 0xAA;                   
		Temp =  bios + 0x2AAA; 
		*Temp = 0x55; 
		Temp =  bios + 0x5555; 
		*Temp = 0xA0;                   
		*dst = *src;
		toggle_ready_jedec(bios);
		if (*dst != *src)
			printf("BAD! dst 0x%lx val 0x%x src 0x%x\n",
			       (unsigned long)dst, *dst, *src);
		dst++, src++;
	}

	return(0);
}

int probe_49fl004 (struct flashchip * flash)
{
        volatile char * bios = flash->virt_addr;
        unsigned char  id1, id2;

        *(volatile char *) (bios + 0x5555) = 0xAA;
        myusec_delay(10);
        *(volatile char *) (bios + 0x2AAA) = 0x55;
        myusec_delay(10);
        *(volatile char *) (bios + 0x5555) = 0x90;

        myusec_delay(10);

        id1 = *(volatile unsigned char *) bios;
        id2 = *(volatile unsigned char *) (bios + 0x01);

        *(volatile char *) (bios + 0x5555) = 0xAA;
        *(volatile char *) (bios + 0x2AAA) = 0x55;
        *(volatile char *) (bios + 0x5555) = 0xF0;

        myusec_delay(10);

	printf("%s: id1 0x%x, id2 0x%x\n", __FUNCTION__, id1, id2);

        if (id1 == flash->manufacture_id && id2 == flash->model_id)
                return 1;

        return 0;
}

int erase_49fl004 (struct flashchip * flash)
{
	volatile unsigned char * bios = flash->virt_addr;
	volatile unsigned char *Temp;

        Temp  = bios + 0x5555; /* set up address to be C000:5555h      */
        *Temp = 0xAA;            /* write data 0xAA to the address       */
	myusec_delay(10);
        Temp  = bios + 0x2AAA; /* set up address to be C000:2AAAh      */
        *Temp = 0x55;         /* write data 0x55 to the address       */
	myusec_delay(10);
        Temp  = bios + 0x5555; /* set up address to be C000:5555h      */
        *Temp = 0x80;            /* write data 0x80 to the address       */
	myusec_delay(10);
        Temp  = bios + 0x5555; /* set up address to be C000:5555h      */
        *Temp = 0xAA;         /* write data 0xAA to the address       */
	myusec_delay(10);
        Temp  = bios + 0x2AAA; /* set up address to be C000:2AAAh      */
        *Temp = 0x55;          /* write data 0x55 to the address       */
	myusec_delay(10);
        Temp  = bios + 0x5555; /* set up address to be C000:5555h      */
        *Temp = 0x10;       /* write data 0x10  to the address       */


        /* wait for Toggle bit ready         */
        toggle_ready_jedec(bios);

	return(0);
}

int write_49fl004 (struct flashchip * flash, unsigned char * buf)
{
	int i;
	int total_size = flash->total_size * 1024, page_size = flash->page_size;
	volatile char * bios = flash->virt_addr;

//	unprotect_49fl004 (bios);
//	erase_49fl004(flash);
	printf ("Programming Page: ");
	for (i = 0; i < total_size/page_size; i++) {
		/* erase the page before programming */
		erase_block_49fl004(bios, i * page_size);

		/* write to the sector */
		printf ("%04d at address: 0x%08x", i, i * page_size);
		write_block_49fl004(bios, buf + i * page_size, bios + i * page_size,
				     page_size);
		printf ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
                fflush(stdout);
	}
	printf("\n");

//	protect_49fl004 (bios);

	return(0);
}
