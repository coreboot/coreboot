#ifndef PC80_MC146818RTC_H
#define PC80_MC146818RTC_H

#if CONFIG_ARCH_X86

#include <arch/io.h>
#include <types.h>

#ifndef RTC_BASE_PORT
#define RTC_BASE_PORT 0x70
#endif

#define RTC_PORT(x)	(RTC_BASE_PORT + (x))

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

#define RTC_BOOT_NORMAL		0x1

/* update-in-progress  - set to "1" 244 microsecs before RTC goes off the bus,
 * reset after update (may take 1.984ms @ 32768Hz RefClock) is complete,
 * totaling to a max high interval of 2.228 ms.
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
  /* Periodic intr. / Square wave rate select. 0 = none, 1 = 32.8kHz,... 15 = 2Hz */
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

/* Date and Time in RTC CMOS */
#define RTC_CLK_SECOND		0
#define RTC_CLK_SECOND_ALARM	1
#define RTC_CLK_MINUTE		2
#define RTC_CLK_MINUTE_ALARM	3
#define RTC_CLK_HOUR		4
#define RTC_CLK_HOUR_ALARM	5
#define RTC_CLK_DAYOFWEEK	6
#define RTC_CLK_DAYOFMONTH	7
#define RTC_CLK_MONTH		8
#define RTC_CLK_YEAR		9
#define RTC_CLK_ALTCENTURY	0x32

/* On PCs, the checksum is built only over bytes 16..45 */
#define PC_CKS_RANGE_START	16
#define PC_CKS_RANGE_END	45
#define PC_CKS_LOC		46

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

/* Upon return the caller is guaranteed 244 microseconds to complete any
 * RTC operations.  wait_uip may be called a single time prior to multiple
 * accesses, but sequences requiring more time should call wait_uip again.
 */
static inline void wait_uip(void)
{
	while (cmos_read(RTC_REG_A) & RTC_UIP)
		;
}

static inline void cmos_write_inner(unsigned char val, unsigned char addr)
{
	int offs = 0;
	if (addr >= 128) {
		offs = 2;
		addr -= 128;
	}
	outb(addr, RTC_BASE_PORT + offs + 0);
	outb(val, RTC_BASE_PORT + offs + 1);
}

static inline void cmos_write(unsigned char val, unsigned char addr)
{
	u8 control_state = cmos_read(RTC_CONTROL);
	/* There are various places where RTC bits might be hiding,
	 * eg. the Century / AltCentury byte. So to be safe, disable
	 * RTC before changing any value.
	 */
	if ((addr != RTC_CONTROL) && !(control_state & RTC_SET)) {
		cmos_write_inner(control_state | RTC_SET, RTC_CONTROL);
	}
	cmos_write_inner(val, addr);
	/* reset to prior configuration */
	if ((addr != RTC_CONTROL) && !(control_state & RTC_SET)) {
		cmos_write_inner(control_state, RTC_CONTROL);
	}
}

static inline void cmos_disable_rtc(void)
{
	u8 control_state = cmos_read(RTC_CONTROL);
	cmos_write(control_state | RTC_SET, RTC_CONTROL);
}

static inline void cmos_enable_rtc(void)
{
	u8 control_state = cmos_read(RTC_CONTROL);
	cmos_write(control_state & ~RTC_SET, RTC_CONTROL);
}

static inline u32 cmos_read32(u8 offset)
{
	u32 value = 0;
	u8 i;
	for (i = 0; i < sizeof(value); ++i)
		value |= cmos_read(offset + i) << (i << 3);
	return value;
}

static inline void cmos_write32(u8 offset, u32 value)
{
	u8 i;
	for (i = 0; i < sizeof(value); ++i)
		cmos_write((value >> (i << 3)) & 0xff, offset + i);
}

#if !defined(__ROMCC__)
void cmos_init(bool invalid);
void cmos_check_update_date(void);

enum cb_err set_option(const char *name, void *val);
enum cb_err get_option(void *dest, const char *name);
unsigned read_option_lowlevel(unsigned start, unsigned size, unsigned def);

#else /* defined(__ROMCC__) */
#include <drivers/pc80/rtc/mc146818rtc_early.c>
#endif /* !defined(__ROMCC__) */
#define read_option(name, default) read_option_lowlevel(CMOS_VSTART_ ##name, CMOS_VLEN_ ##name, (default))

#if CONFIG_CMOS_POST
#if CONFIG_USE_OPTION_TABLE
# include "option_table.h"
# define CMOS_POST_OFFSET (CMOS_VSTART_cmos_post_offset >> 3)
#else
# if defined(CONFIG_CMOS_POST_OFFSET) && CONFIG_CMOS_POST_OFFSET
#  define CMOS_POST_OFFSET CONFIG_CMOS_POST_OFFSET
# else
#  error "Must configure CONFIG_CMOS_POST_OFFSET"
# endif
#endif

/*
 *    0 = Bank Select Magic
 *    1 = Bank 0 POST
 *    2 = Bank 1 POST
 *  3-6 = BANK 0 Extra log
 * 7-10 = BANK 1 Extra log
 */
#define CMOS_POST_BANK_OFFSET     (CMOS_POST_OFFSET)
#define CMOS_POST_BANK_0_MAGIC    0x80
#define CMOS_POST_BANK_0_OFFSET   (CMOS_POST_OFFSET + 1)
#define CMOS_POST_BANK_0_EXTRA    (CMOS_POST_OFFSET + 3)
#define CMOS_POST_BANK_1_MAGIC    0x81
#define CMOS_POST_BANK_1_OFFSET   (CMOS_POST_OFFSET + 2)
#define CMOS_POST_BANK_1_EXTRA    (CMOS_POST_OFFSET + 7)

#define CMOS_POST_EXTRA_DEV_PATH  0x01

void cmos_post_log(void);

/* cmos_post_init() is exposed in this manner because it also needs to be called
 * by bootblock code compiled by romcc. */
static inline void cmos_post_init(void)
{
	u8 magic = CMOS_POST_BANK_0_MAGIC;

	/* Switch to the other bank */
	switch (cmos_read(CMOS_POST_BANK_OFFSET)) {
	case CMOS_POST_BANK_1_MAGIC:
		break;
	case CMOS_POST_BANK_0_MAGIC:
		magic = CMOS_POST_BANK_1_MAGIC;
		break;
	default:
		/* Initialize to zero */
		cmos_write(0, CMOS_POST_BANK_0_OFFSET);
		cmos_write(0, CMOS_POST_BANK_1_OFFSET);
#if CONFIG_CMOS_POST_EXTRA
		cmos_write32(CMOS_POST_BANK_0_EXTRA, 0);
		cmos_write32(CMOS_POST_BANK_1_EXTRA, 0);
#endif
	}

	cmos_write(magic, CMOS_POST_BANK_OFFSET);
}
#else
static inline void cmos_post_log(void) {}
static inline void cmos_post_init(void) {}
#endif /* CONFIG_CMOS_POST */

#else /* !CONFIG_ARCH_X86 */
static inline void cmos_post_init(void) {}
#endif /* CONFIG_ARCH_X86 */

#endif /*  PC80_MC146818RTC_H */
