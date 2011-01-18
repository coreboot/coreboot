#include <console/console.h>
#include <pc80/mc146818rtc.h>
#include <boot/coreboot_tables.h>
#include <string.h>
#if CONFIG_USE_OPTION_TABLE
#include "option_table.h"
#include <cbfs.h>
#endif

/* control registers - Moto names
 */
#define RTC_REG_A		10
#define RTC_REG_B		11
#define RTC_REG_C		12
#define RTC_REG_D		13


/**********************************************************************
 * register details
 **********************************************************************/
#define RTC_FREQ_SELECT	RTC_REG_A

/* update-in-progress  - set to "1" 244 microsecs before RTC goes off the bus,
 * reset after update (may take 1.984ms @ 32768Hz RefClock) is complete,
 * totalling to a max high interval of 2.228 ms.
 */
# define RTC_UIP		0x80
# define RTC_DIV_CTL		0x70
   /* divider control: refclock values 4.194 / 1.049 MHz / 32.768 kHz */
#  define RTC_REF_CLCK_4MHZ	0x00
#  define RTC_REF_CLCK_1MHZ	0x10
#  define RTC_REF_CLCK_32KHZ	0x20
   /* 2 values for divider stage reset, others for "testing purposes only" */
#  define RTC_DIV_RESET1	0x60
#  define RTC_DIV_RESET2	0x70
  /* Periodic intr. / Square wave rate select. 0=none, 1=32.8kHz,... 15=2Hz */
# define RTC_RATE_SELECT 	0x0F
#  define RTC_RATE_NONE		0x00
#  define RTC_RATE_32786HZ	0x01
#  define RTC_RATE_16384HZ	0x02
#  define RTC_RATE_8192HZ	0x03
#  define RTC_RATE_4096HZ	0x04
#  define RTC_RATE_2048HZ	0x05
#  define RTC_RATE_1024HZ	0x06
#  define RTC_RATE_512HZ	0x07
#  define RTC_RATE_256HZ	0x08
#  define RTC_RATE_128HZ	0x09
#  define RTC_RATE_64HZ		0x0a
#  define RTC_RATE_32HZ		0x0b
#  define RTC_RATE_16HZ		0x0c
#  define RTC_RATE_8HZ		0x0d
#  define RTC_RATE_4HZ		0x0e
#  define RTC_RATE_2HZ		0x0f

/**********************************************************************/
#define RTC_CONTROL	RTC_REG_B
# define RTC_SET 0x80		/* disable updates for clock setting */
# define RTC_PIE 0x40		/* periodic interrupt enable */
# define RTC_AIE 0x20		/* alarm interrupt enable */
# define RTC_UIE 0x10		/* update-finished interrupt enable */
# define RTC_SQWE 0x08		/* enable square-wave output */
# define RTC_DM_BINARY 0x04	/* all time/date values are BCD if clear */
# define RTC_24H 0x02		/* 24 hour mode - else hours bit 7 means pm */
# define RTC_DST_EN 0x01	/* auto switch DST - works f. USA only */

/**********************************************************************/
#define RTC_INTR_FLAGS	RTC_REG_C
/* caution - cleared by read */
# define RTC_IRQF 0x80		/* any of the following 3 is active */
# define RTC_PF 0x40
# define RTC_AF 0x20
# define RTC_UF 0x10

/**********************************************************************/
#define RTC_VALID	RTC_REG_D
# define RTC_VRT 0x80		/* valid RAM and time */
/**********************************************************************/

#if CONFIG_USE_OPTION_TABLE
static int rtc_checksum_valid(int range_start, int range_end, int cks_loc)
{
	int i;
	unsigned sum, old_sum;
	sum = 0;
	for(i = range_start; i <= range_end; i++) {
		sum += cmos_read(i);
	}
	sum = (~sum)&0x0ffff;
	old_sum = ((cmos_read(cks_loc)<<8) | cmos_read(cks_loc+1))&0x0ffff;
	return sum == old_sum;
}

static void rtc_set_checksum(int range_start, int range_end, int cks_loc)
{
	int i;
	unsigned sum;
	sum = 0;
	for(i = range_start; i <= range_end; i++) {
		sum += cmos_read(i);
	}
	sum = ~(sum & 0x0ffff);
	cmos_write(((sum >> 8) & 0x0ff), cks_loc);
	cmos_write(((sum >> 0) & 0x0ff), cks_loc+1);
}
#endif

#if CONFIG_ARCH_X86
#define RTC_CONTROL_DEFAULT (RTC_24H)
#define RTC_FREQ_SELECT_DEFAULT (RTC_REF_CLCK_32KHZ | RTC_RATE_1024HZ)
#else
#if CONFIG_ARCH_ALPHA
#define RTC_CONTROL_DEFAULT (RTC_SQWE | RTC_24H)
#define RTC_FREQ_SELECT_DEFAULT (RTC_REF_CLCK_32KHZ | RTC_RATE_1024HZ)
#endif
#endif

void rtc_init(int invalid)
{
#if CONFIG_USE_OPTION_TABLE
	unsigned char x;
	int cmos_invalid, checksum_invalid;
#endif

	printk(BIOS_DEBUG, "RTC Init\n");

#if CONFIG_USE_OPTION_TABLE
	/* See if there has been a CMOS power problem. */
	x = cmos_read(RTC_VALID);
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
		cmos_write(0, 0x01);
		cmos_write(0, 0x03);
		cmos_write(0, 0x05);
		for(i = 10; i < 48; i++) {
			cmos_write(0, i);
		}

		if (cmos_invalid) {
			/* Now setup a default date of Sat 1 January 2000 */
			cmos_write(0, 0x00); /* seconds */
			cmos_write(0, 0x02); /* minutes */
			cmos_write(1, 0x04); /* hours */
			cmos_write(7, 0x06); /* day of week */
			cmos_write(1, 0x07); /* day of month */
			cmos_write(1, 0x08); /* month */
			cmos_write(0, 0x09); /* year */
		}
#endif
	}
#endif

	/* Setup the real time clock */
	cmos_write(RTC_CONTROL_DEFAULT, RTC_CONTROL);
	/* Setup the frequency it operates at */
	cmos_write(RTC_FREQ_SELECT_DEFAULT, RTC_FREQ_SELECT);

#if CONFIG_USE_OPTION_TABLE
	/* See if there is a LB CMOS checksum error */
	checksum_invalid = !rtc_checksum_valid(LB_CKS_RANGE_START,
			LB_CKS_RANGE_END,LB_CKS_LOC);
	if(checksum_invalid)
		printk(BIOS_DEBUG, "Invalid CMOS LB checksum\n");

	/* Make certain we have a valid checksum */
	rtc_set_checksum(PC_CKS_RANGE_START,
                        PC_CKS_RANGE_END,PC_CKS_LOC);
#endif

	/* Clear any pending interrupts */
	(void) cmos_read(RTC_INTR_FLAGS);
}


#if CONFIG_USE_OPTION_TABLE
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
		uchar = cmos_read(byte); /* load the byte */
		uchar >>= byte_bit;	/* shift the bits to byte align */
		/* clear unspecified bits */
		ret[0] = uchar & ((1 << length) -1);
	}
	else {	/* more that one byte so transfer the whole bytes */
		for(i=0;length;i++,length-=8,byte++) {
			/* load the byte */
			ret[i]=cmos_read(byte);
		}
	}
	return 0;
}

int get_option(void *dest, const char *name)
{
	struct cmos_option_table *ct;
	struct cmos_entries *ce;
	size_t namelen;
	int found=0;

	/* Figure out how long name is */
	namelen = strnlen(name, CMOS_MAX_NAME_LENGTH);

	/* find the requested entry record */
	ct=cbfs_find_file("cmos_layout.bin", CMOS_COMPONENT_CMOS_LAYOUT);
	ce=(struct cmos_entries*)((unsigned char *)ct + ct->header_length);
	for(;ce->tag==LB_TAG_OPTION;
		ce=(struct cmos_entries*)((unsigned char *)ce + ce->size)) {
		if (memcmp(ce->name, name, namelen) == 0) {
			found=1;
			break;
		}
	}
	if(!found) {
		printk(BIOS_DEBUG, "WARNING: No CMOS option '%s'.\n", name);
		return(-2);
	}

	if(get_cmos_value(ce->bit, ce->length, dest))
		return(-3);
	if(!rtc_checksum_valid(LB_CKS_RANGE_START,
			LB_CKS_RANGE_END,LB_CKS_LOC))
		return(-4);
	return(0);
}
#endif /* CONFIG_USE_OPTION_TABLE */
