#include <console/console.h>
#include <arch/io.h>
#include <pc80/mc146818rtc.h>
#include <boot/coreboot_tables.h>
#include <string.h>

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

static inline unsigned char cmos_read(unsigned char addr)
{
	int offs = 0;
	if (addr >= 128) {
		offs = 2;
		addr -= 128;
	}
	outb(addr, RTC_BASE_PORT + offs + 0);
	return inb(RTC_BASE_PORT + offs + 1);
}

static inline void cmos_write(unsigned char val, unsigned char addr)
{
	int offs = 0;
	if (addr >= 128) {
		offs = 2;
		addr -= 128;
	}
	outb(addr, RTC_BASE_PORT + offs + 0);
	outb(val, RTC_BASE_PORT + offs + 1);
}

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

#define RTC_CONTROL_DEFAULT (RTC_24H)
#define RTC_FREQ_SELECT_DEFAULT (RTC_REF_CLCK_32KHZ | RTC_RATE_1024HZ)

#if 0 /* alpha setup */
#undef RTC_CONTROL_DEFAULT
#undef RTC_FREQ_SELECT_DEFAULT
#define RTC_CONTROL_DEFAULT (RTC_SQWE | RTC_24H)
#define RTC_FREQ_SELECT_DEFAULT (RTC_REF_CLCK_32KHZ | RTC_RATE_1024HZ)
#endif

void rtc_init(int invalid)
{
	unsigned char x;
	int cmos_invalid, checksum_invalid;

	printk_debug("RTC Init\n");

#if HAVE_OPTION_TABLE
	/* See if there has been a CMOS power problem. */
	x = cmos_read(RTC_VALID);
	cmos_invalid = !(x & RTC_VRT);

	/* See if there is a CMOS checksum error */
	checksum_invalid = !rtc_checksum_valid(PC_CKS_RANGE_START,
			PC_CKS_RANGE_END,PC_CKS_LOC);

	if (invalid || cmos_invalid || checksum_invalid) {
		printk_warning("RTC:%s%s%s zeroing cmos\n",
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

#if HAVE_OPTION_TABLE
	/* See if there is a LB CMOS checksum error */
	checksum_invalid = !rtc_checksum_valid(LB_CKS_RANGE_START,
			LB_CKS_RANGE_END,LB_CKS_LOC);
	if(checksum_invalid)
		printk_debug("Invalid CMOS LB checksum\n");

	/* Make certain we have a valid checksum */
	rtc_set_checksum(PC_CKS_RANGE_START,
                        PC_CKS_RANGE_END,PC_CKS_LOC);
#endif

	/* Clear any pending interrupts */
	(void) cmos_read(RTC_INTR_FLAGS);
}


#if USE_OPTION_TABLE == 1
/*
 * Functions to save/return values stored in the 256byte cmos.
 *
 * To be able to use space maximally we want to only store as many bits as
 * needed, and not be limited by byte boundaries. We therefor clamp the size
 * down to an unsigned int. Since the values that we are allowed to touch are
 * either an enum or a hexadecimal value, this size should suit most purposes.
 *
 * These two functions are doing bitshifting, and are therefor a bit
 * nontrivial. To understand these operations, first read the ones outside the
 * loop. The ones inside the loop are just adding i to the same calculations,
 * with the shift twice inverted, as negative shifts aren't nice.
 */
static unsigned int
get_cmos_value(int bit, int length)
{
    unsigned int tmp;
    int i;

    /* negative left shift --> right shift */
    tmp = cmos_read(bit / 8) >> (bit % 8);

    for (i = 1; (8 * i) < ((bit % 8) + length); i++)
	tmp |= cmos_read((bit / 8) + i) << ((8 * i) - (bit % 8));

    /* 1 << 32 - 1 isn't cool inside an int */
    if (length != 32)
	tmp &= (1 << length) - 1;

    return tmp;
}

static void
set_cmos_value(int bit, int length, unsigned int value)
{
	unsigned int mask;
	unsigned char cmos;
	int i;

	/* 1 << 32 - 1 isn't cool inside an int */
	if (length != 32)
	    mask = (1 << length) - 1;
	else
	    mask = -1;

	value &= mask;

	/* negative right shifts --> left shifts */
	cmos = cmos_read(bit / 8);
	cmos &= ~(mask << (bit % 8));
	cmos |= value << (bit % 8);
	cmos_write(cmos, bit / 8);

	for (i = 1; (8 * i) < ((bit % 8) + length); i++) {
		cmos = cmos_read((bit / 8) + i);
		cmos &= ~(mask >> ((8 * i) - (bit % 8)));
		cmos |= value >> ((8 * i) - (bit % 8));
		cmos_write(cmos, (bit / 8) + i);
	}
}

int
get_option(char *name, unsigned int *value)
{
	extern struct cmos_option_table option_table;
	struct cmos_option_table *ct;
	struct cmos_entries *ce;
	size_t namelen;
	int found=0;

	/* Figure out how long name is */
	namelen = strnlen(name, CMOS_MAX_NAME_LENGTH);

	/* find the requested entry record */
	ct=&option_table;
	ce=(struct cmos_entries*)((unsigned char *)ct + ct->header_length);
	for(;ce->tag==LB_TAG_OPTION;
		ce=(struct cmos_entries*)((unsigned char *)ce + ce->size)) {
		if (memcmp(ce->name, name, namelen) == 0) {
			found=1;
			break;
		}
	}
	if(!found) {
		printk_err("ERROR: No cmos option '%s'\n", name);
		return(-2);
	}

	if (ce->length > 32) {
		printk_err("ERROR: cmos option '%s' is too large.\n", name);
		return -3;
	}


	*value = get_cmos_value(ce->bit, ce->length);

	if(!rtc_checksum_valid(LB_CKS_RANGE_START,
			       LB_CKS_RANGE_END,LB_CKS_LOC))
		return(-4);
	return(0);
}

int
set_option(char *name, unsigned int value)
{
	extern struct cmos_option_table option_table;
	struct cmos_option_table *ct;
	struct cmos_entries *ce;
	size_t namelen;
	int found = 0;

	/* Figure out how long name is */
	namelen = strnlen(name, CMOS_MAX_NAME_LENGTH);

	/* find the requested entry record */
	ct = &option_table;
	ce = (struct cmos_entries*) ((unsigned char *) ct + ct->header_length);

	for(;ce->tag==LB_TAG_OPTION;
		ce=(struct cmos_entries*)((unsigned char *)ce + ce->size)) {
		if (memcmp(ce->name, name, namelen) == 0) {
			found=1;
			break;
		}
	}

	if (!found) {
		printk_err("ERROR: Unknown cmos option '%s'\n", name);
		return(-2);
	}

	if (ce->length > 32) {
		printk_err("ERROR: cmos option '%s' is too large.\n", name);
		return -3;
	}

	set_cmos_value(ce->bit, ce->length, value);

	/* We should not update the checksum here. */

	return 0;
}
#else
int
get_option(char *name, unsigned int *value)
{
	return -2;
}

int
set_option(char *name, unsigned int value)
{
	return -2;
}

#endif /* USE_OPTION_TABLE */
