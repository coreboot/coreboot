#ifndef PC80_MC146818RTC_H
#define PC80_MC146818RTC_H

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

/* On PCs, the checksum is built only over bytes 16..45 */
#define PC_CKS_RANGE_START	16
#define PC_CKS_RANGE_END	45
#define PC_CKS_LOC		46

#ifndef UTIL_BUILD_OPTION_TABLE
#include <arch/io.h>
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
#endif

#if !defined(__ROMCC__)
void rtc_init(int invalid);
#if CONFIG_USE_OPTION_TABLE
int get_option(void *dest, const char *name);
unsigned read_option_lowlevel(unsigned start, unsigned size, unsigned def);
#else
static inline int get_option(void *dest __attribute__((unused)),
	const char *name __attribute__((unused))) { return -2; }
static inline unsigned read_option_lowlevel(unsigned start, unsigned size, unsigned def)
	{ return def; }
#endif
#else
#include <pc80/mc146818rtc_early.c>
#endif
#define read_option(name, default) read_option_lowlevel(CMOS_VSTART_ ##name, CMOS_VLEN_ ##name, (default))

#endif /*  PC80_MC146818RTC_H */
