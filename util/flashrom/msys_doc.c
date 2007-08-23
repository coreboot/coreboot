/*
 * msys_doc.c: driver for programming m-systems doc devices
 *
 *
 * Copyright 2003   Niki W. Waibel <niki.waibel@gmx.net>
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
 */

#include <stdio.h>
#include <unistd.h>
#include "flash.h"
#include "msys_doc.h"

static int doc_wait(volatile uint8_t *bios, int timeout);
static uint8_t doc_read_chipid(volatile uint8_t *bios);
static uint8_t doc_read_docstatus(volatile uint8_t *bios);
static uint8_t doc_read_cdsncontrol(volatile uint8_t *bios);
static void doc_write_cdsncontrol(volatile uint8_t *bios, uint8_t data);

int probe_md2802(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;
	uint8_t chipid;
#ifndef MSYSTEMS_DOC_NO_55AA_CHECKING
	uint8_t id_0x55, id_0xAA;
#endif				/* !MSYSTEMS_DOC_NO_55AA_CHECKING */
	int i, toggle_a, toggle_b;

	printf_debug("%s:\n", __FUNCTION__);
	printf_debug("%s: *******************************\n", __FUNCTION__);
	printf_debug("%s: * THIS IS A PRE ALPHA VERSION *\n", __FUNCTION__);
	printf_debug("%s: * IN THE DEVELOPEMENT *********\n", __FUNCTION__);
	printf_debug("%s: * PROCESS RIGHT NOW. **********\n", __FUNCTION__);
	printf_debug("%s: *******************************\n", __FUNCTION__);
	printf_debug("%s: * IF YOU ARE NOT A DEVELOPER **\n", __FUNCTION__);
	printf_debug("%s: * THEN DO NOT TRY TO READ OR **\n", __FUNCTION__);
	printf_debug("%s: * WRITE TO THIS DEVICE ********\n", __FUNCTION__);
	printf_debug("%s: *******************************\n", __FUNCTION__);
	printf_debug("%s:\n", __FUNCTION__);

	printf_debug("%s: switching off reset mode ...\n", __FUNCTION__);
	doc_write(0x85, bios, DOCControl);
	doc_write(0x85, bios, DOCControl);
	doc_read_4nop(bios);
	if (doc_wait(bios, 5000))
		return (-1);
	printf("%s: switching off reset mode ... done\n", __FUNCTION__);
	printf("%s:\n", __FUNCTION__);

	printf("%s: switching off write protection ...\n", __FUNCTION__);
	doc_write_cdsncontrol(bios, doc_read_cdsncontrol(bios) & (~0x08));
	printf("%s: switching off write protection ... done\n", __FUNCTION__);
	printf("%s:\n", __FUNCTION__);

	chipid = doc_read_chipid(bios);
#ifndef MSYSTEMS_DOC_NO_55AA_CHECKING
	id_0x55 = doc_read(bios, IPL_0x0000);
	id_0xAA = doc_read(bios, IPL_0x0001);
#endif				/* !MSYSTEMS_DOC_NO_55AA_CHECKING */
	printf("%s: IPL_0x0000: 0x%02x\n", __FUNCTION__, id_0x55);
	printf("%s: IPL_0x0001: 0x%02x\n", __FUNCTION__, id_0xAA);
	printf("%s: IPL_0x0002: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, IPL_0x0002));
	printf("%s: IPL_0x0003: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, IPL_0x0003));
	printf("%s:\n", __FUNCTION__);
	printf("%s: ChipID: 0x%02x\n", __FUNCTION__, chipid);
	printf("%s: DOCStatus: 0x%02x\n", __FUNCTION__,
	       doc_read_docstatus(bios));
	printf("%s: FloorSelect: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, FloorSelect));
	printf("%s: CDSNControl: 0x%02x\n", __FUNCTION__,
	       doc_read_cdsncontrol(bios));
	printf("%s: CDSNDeviceSelect: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, CDSNDeviceSelect));
	printf("%s: ECCConfiguration: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, ECCConfiguration));
	printf("%s: CDSNSlowIO: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, CDSNSlowIO));
	printf("%s: ECCSyndrome0: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, ECCSyndrome0));
	printf("%s: ECCSyndrome1: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, ECCSyndrome1));
	printf("%s: ECCSyndrome2: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, ECCSyndrome2));
	printf("%s: ECCSyndrome3: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, ECCSyndrome3));
	printf("%s: ECCSyndrome4: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, ECCSyndrome4));
	printf("%s: ECCSyndrome5: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, ECCSyndrome5));
	printf("%s: AliasResolution: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, AliasResolution));
	printf("%s: ConfigurationInput: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, ConfigurationInput));
	printf("%s: ReadPipelineInitialization: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, ReadPipelineInitialization));
	printf("%s: LastDataRead: 0x%02x\n", __FUNCTION__,
	       doc_read(bios, LastDataRead));
	printf("%s:\n", __FUNCTION__);

	printf("%s: checking ECCConfiguration toggle bit\n", __FUNCTION__);
	printf("%s:", __FUNCTION__);
	toggle_a = toggle_b = 0;
	for (i = 0; i < 10; i++) {
		uint8_t toggle = doc_toggle(bios);

		printf(" 0x%02x", toggle);

		if (i % 2)
			toggle_a += toggle;
		else
			toggle_b += toggle;
	}			/* for(i=0; i<10; i++) */
	printf("\n%s: toggle result: %d/%d\n", __FUNCTION__, toggle_a,
	       toggle_b);

	if (chipid == flash->model_id && ((toggle_a == 5 && toggle_b == 0)
					  || (toggle_a == 0 && toggle_b == 5))
#ifndef MSYSTEMS_DOC_NO_55AA_CHECKING
	    && id_0x55 == 0x55 && id_0xAA == 0xaa
#endif				/* !MSYSTEMS_DOC_NO_55AA_CHECKING */
	    ) {
		return (1);
	}

	return (0);
}				/* int probe_md2802(struct flashchip *flash) */

int read_md2802(struct flashchip *flash, uint8_t *buf)
{

	return (0);
}				/* int read_md2802(struct flashchip *flash, uint8_t *buf) */

int erase_md2802(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;

	return (1);
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0x80;

	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0x10;
}				/* int erase_md2802(struct flashchip *flash) */

int write_md2802(struct flashchip *flash, uint8_t *buf)
{
	int i;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	return (1);
	erase_md2802(flash);
	if (*bios != (uint8_t) 0xff) {
		printf("ERASE FAILED\n");
		return -1;
	}
	printf("Programming Page: ");
	for (i = 0; i < total_size / page_size; i++) {
		printf("%04d at address: 0x%08x", i, i * page_size);
		//write_page_md2802(bios, buf + i * page_size, bios + i * page_size, page_size);
		printf
		    ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
	printf("\n");
	//protect_md2802(bios);

	return 0;
}				/* int write_md2802(struct flashchip *flash, uint8_t *buf) */

/*
	wait timeout msec for doc to become ready
	return:
		0: ready
		-1: timeout expired
*/
static int doc_wait(volatile uint8_t *bios, int timeout)
{
	int i = 20;

	doc_read_4nop(bios);

	while (_doc_busy(bios) && (i != 0)) {
		usleep(timeout * 1000 / 20);
		i--;
	}

	doc_read_2nop(bios);

	if (_doc_busy(bios)) {
		doc_read_2nop(bios);
		return (-1);
	}

	return (0);
}				/* static int doc_wait(volatile uint8_t *bios, int timeout) */

static uint8_t doc_read_docstatus(volatile uint8_t *bios)
{
	doc_read(bios, CDSNSlowIO);
	doc_read_2nop(bios);

	return (doc_read(bios, _DOCStatus));
}				/* static uint8_t doc_read_docstatus(volatile uint8_t *bios) */

static uint8_t doc_read_chipid(volatile uint8_t *bios)
{
	doc_read(bios, CDSNSlowIO);
	doc_read_2nop(bios);

	return (doc_read(bios, _ChipID));
}				/* static uint8_t doc_read_chipid(volatile uint8_t *bios) */

static uint8_t doc_read_cdsncontrol(volatile uint8_t *bios)
{
	uint8_t value;

	/* the delays might be necessary when reading the busy bit,
	   but because a read to this reg reads the busy bit
	   anyway we better do this delays... */
	doc_read_4nop(bios);
	value = doc_read(bios, _CDSNControl);
	doc_read_2nop(bios);

	return (value);
}				/* static uint8_t doc_read_chipid(volatile char *bios) */

static void doc_write_cdsncontrol(volatile uint8_t *bios, uint8_t data)
{
	doc_write(data, bios, _CDSNControl);
	doc_read_4nop(bios);
}				/* static void doc_write_chipid(volatile char *bios, uint8_t data) */
