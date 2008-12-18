/*
 *
 * Copyright (C) 200X FIXME
 * Copyright (C) 2007 coresystems GmbH
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 *
 */


#include <types.h>
#include <io.h>
#include <console.h>
#include <tables.h>
#include <mc146818rtc.h>
#include <string.h>
#include <lar.h>

/*
 * Why is this not a function? I assume the two io 
 * operations are less overhead than a function call.
 */

#define CMOS_READ(addr) ({ \
outb((addr),RTC_PORT(0)); \
inb(RTC_PORT(1)); \
})

#define CMOS_WRITE(val, addr) ({ \
outb((addr),RTC_PORT(0)); \
outb((val),RTC_PORT(1)); \
})

#ifdef CONFIG_OPTION_TABLE

static int rtc_checksum_valid(int range_start, int range_end, int cks_loc)
{
	int i;
	unsigned sum, old_sum;
	sum = 0;
	for(i = range_start; i <= range_end; i++) {
		sum += CMOS_READ(i);
	}
	sum = (~sum)&0x0ffff;
	old_sum = ((CMOS_READ(cks_loc)<<8) | CMOS_READ(cks_loc+1))&0x0ffff;
	return sum == old_sum;
}

static void rtc_set_checksum(int range_start, int range_end, int cks_loc)
{
	int i;
	unsigned sum;
	sum = 0;
	for(i = range_start; i <= range_end; i++) {
		sum += CMOS_READ(i);
	}
	sum = ~(sum & 0x0ffff);
	CMOS_WRITE(((sum >> 8) & 0x0ff), cks_loc);
	CMOS_WRITE(((sum >> 0) & 0x0ff), cks_loc+1);
}

#endif

#define RTC_CONTROL_DEFAULT (RTC_24H)
#define RTC_FREQ_SELECT_DEFAULT (RTC_REF_CLCK_32KHZ | RTC_RATE_1024HZ)

void rtc_init(int invalid)
{
#ifdef CONFIG_OPTION_TABLE
	unsigned char x;
	int cmos_invalid, checksum_invalid;
#endif

	printk(BIOS_DEBUG, "Initializing realtime clock.\n");

#ifdef CONFIG_OPTION_TABLE

	/* See if there has been a CMOS power problem. */
	x = CMOS_READ(RTC_VALID);
	cmos_invalid = !(x & RTC_VRT);

	/* See if there is a CMOS checksum error */
	checksum_invalid = !rtc_checksum_valid(PC_CKS_RANGE_START,
			PC_CKS_RANGE_END,PC_CKS_LOC);

	if (invalid || cmos_invalid || checksum_invalid) {
		printk(BIOS_WARNING, "RTC:%s%s%s zeroing cmos\n",
			invalid?" Clear requested":"", 
			cmos_invalid?" Power Problem":"",
			checksum_invalid?" Checksum invalid":"");
#if 0
		CMOS_WRITE(0, 0x01);
		CMOS_WRITE(0, 0x03);
		CMOS_WRITE(0, 0x05);
		for(i = 10; i < 48; i++) {
			CMOS_WRITE(0, i);
		}
		
		if (cmos_invalid) {
			/* Now setup a default date of Sat 1 January 2000 */
			CMOS_WRITE(0, 0x00); /* seconds */
			CMOS_WRITE(0, 0x02); /* minutes */
			CMOS_WRITE(1, 0x04); /* hours */
			CMOS_WRITE(7, 0x06); /* day of week */
			CMOS_WRITE(1, 0x07); /* day of month */
			CMOS_WRITE(1, 0x08); /* month */
			CMOS_WRITE(0, 0x09); /* year */
		}
#endif
	}
#endif

	/* Setup the real time clock */
	CMOS_WRITE(RTC_CONTROL_DEFAULT, RTC_CONTROL);
	/* Setup the frequency it operates at */
	CMOS_WRITE(RTC_FREQ_SELECT_DEFAULT, RTC_FREQ_SELECT);

#ifdef CONFIG_OPTION_TABLE
	/* See if there is a coreboot CMOS checksum error */
	checksum_invalid = !rtc_checksum_valid(CB_CKS_RANGE_START,
			CB_CKS_RANGE_END,CB_CKS_LOC);
	if(checksum_invalid)
		printk(BIOS_WARNING, "Invalid coreboot CMOS checksum.\n");

	/* Make certain we have a valid checksum */
	rtc_set_checksum(PC_CKS_RANGE_START,
                        PC_CKS_RANGE_END,PC_CKS_LOC);
#endif

	/* Clear any pending interrupts */
	(void) CMOS_READ(RTC_INTR_FLAGS);
}


/* This routine returns the value of the requested bits
	input bit = bit count from the beginning of the cmos image
	      length = number of bits to include in the value
	      ret = a character pointer to where the value is to be returned
	output the value placed in ret
	      returns 0 = successful, -1 = an error occurred
*/
static int get_cmos_value(unsigned long bit, unsigned long length, void *vret)
{
	unsigned char *ret;
	unsigned long byte,byte_bit;
	unsigned long i;
	unsigned char uchar;

	/* The table is checked when it is built to ensure all 
		values are valid. */
	ret = vret;
	byte=bit/8;	/* find the byte where the data starts */
	byte_bit=bit%8; /* find the bit in the byte where the data starts */
	if(length<9) {	/* one byte or less */
		uchar = CMOS_READ(byte); /* load the byte */
		uchar >>= byte_bit;	/* shift the bits to byte align */
		/* clear unspecified bits */
		ret[0] = uchar & ((1 << length) -1);
	}
	else {	/* more that one byte so transfer the whole bytes */
		for(i=0;length;i++,length-=8,byte++) {
			/* load the byte */
			ret[i]=CMOS_READ(byte);
		}
	}
	return 0;
}

struct cmos_option_table *get_option_table(void)
{
	struct mem_file result, archive;
	int ret;

	init_archive(&archive);

	ret = find_file(&archive, "normal/option_table", &result);
	if (ret) {
		printk(BIOS_ERR, "No such file '%s'.\n",
				"normal/option_table");
		return (struct cmos_option_table *)NULL;
	}
	return (struct cmos_option_table *) result.start;
}

int get_option(void *dest, char *name)
{
#if CONFIG_OPTION_TABLE
	// struct cmos_option_table *option_table;
	struct cmos_option_table *ct;
	struct cmos_entries *ce;
	size_t namelen;
	int found=0;
	/* Figure out how long name is */
	namelen = strnlen(name, CMOS_MAX_NAME_LENGTH);
	
	/* find the requested entry record */
	ct=get_option_table();
	ce=(struct cmos_entries*)((unsigned char *)ct + ct->header_length);
	for(;ce->tag==LB_TAG_OPTION;
		ce=(struct cmos_entries*)((unsigned char *)ce + ce->size)) {
		if (memcmp(ce->name, name, namelen) == 0) {
			found=1;
			break;
		}
	}
	if(!found) {
		printk(BIOS_ERR, "ERROR: No cmos option '%s'\n", name);
		return(-2);
	}
	
	if(get_cmos_value(ce->bit, ce->length, dest))
		return(-3);
	if(!rtc_checksum_valid(CB_CKS_RANGE_START,
			CB_CKS_RANGE_END,CB_CKS_LOC))
		return(-4);
	return(0);
#else
	return -2;
#endif
}

/* -------------------------------------------------------------- */

// FIXME: Early starts here

#ifndef MAX_REBOOT_CNT
#define MAX_REBOOT_CNT 2
#endif

static int cmos_error(void)
{
	u8 reg_d;
	/* See if the cmos error condition has been flagged */
	reg_d = CMOS_READ(RTC_REG_D);
	return (reg_d & RTC_VRT) == 0;
}

static int cmos_chksum_valid(void)
{
	u8 addr;
	u32 sum, old_sum;
	sum = 0;
	/* Comput the cmos checksum */
	for(addr = CB_CKS_RANGE_START; addr <= CB_CKS_RANGE_END; addr++) {
		sum += CMOS_READ(addr);
	}
	sum = (sum & 0xffff) ^ 0xffff;

	/* Read the stored checksum */
	old_sum = CMOS_READ(CB_CKS_LOC) << 8;
	old_sum |=  CMOS_READ(CB_CKS_LOC+1);

	return sum == old_sum;
}


int last_boot_normal(void)
{
	u8 byte;

	byte = CMOS_READ(RTC_BOOT_BYTE);
	return (byte & RTC_LAST_BOOT_FLAG_SET);
}

/**
  * Check CMOS for normal or fallback boot mode.
  * Use lxbios to set normal mode once the system is operational.
  */
int check_normal_boot_flag(void)
{
	u8 byte;

	if (cmos_error() || !cmos_chksum_valid()) {
		/* There are no impossible values, no cheksums so just
		 * trust whatever value we have in the the cmos,
		 * but clear the fallback bit.
		 */
		byte = CMOS_READ(RTC_BOOT_BYTE);
		byte &= ~(0x0f << RTC_BOOT_COUNT_SHIFT
		 | RTC_LAST_BOOT_FLAG_SET | RTC_NORMAL_BOOT_FLAG_SET);
		byte |= MAX_REBOOT_CNT << RTC_BOOT_COUNT_SHIFT;
		CMOS_WRITE(byte, RTC_BOOT_BYTE);
	}

	/* The RTC_BOOT_BYTE is now o.k. see where to go. */
	byte = CMOS_READ(RTC_BOOT_BYTE);
	
	/* Are we in normal mode? */
	if (byte & RTC_NORMAL_BOOT_FLAG_SET) {
		byte &= ~(0x0f << RTC_BOOT_COUNT_SHIFT); /* yes, clear the boot count */
	}

	/* Properly set the last boot flag */
	byte &= ~(RTC_LAST_BOOT_FLAG_SET | RTC_NORMAL_BOOT_FLAG_SET);
	if ((byte >> 4) < MAX_REBOOT_CNT) {
		byte |= RTC_LAST_BOOT_FLAG_SET;
	}

	/* Are we already at the max count? */
	if ((byte >> RTC_BOOT_COUNT_SHIFT) < MAX_REBOOT_CNT) {
		byte += 1 << RTC_BOOT_COUNT_SHIFT; /* No, add 1 to the count */
	}
	else {
		byte &= ~RTC_NORMAL_BOOT_FLAG_SET;	/* Yes, put in fallback mode */
	}

	/* Save the boot byte */
	CMOS_WRITE(byte, RTC_BOOT_BYTE);

	return (byte & RTC_NORMAL_BOOT_FLAG_SET);
}

unsigned read_option(unsigned start, unsigned size, unsigned def)
{
#ifdef CONFIG_OPTION_TABLE
	unsigned byte;
	byte = CMOS_READ(start/8);
	return (byte >> (start & 7U)) & ((1U << size) - 1U);
#else
	return def;
#endif
}

