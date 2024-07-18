/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef PC80_MC146818RTC_H
#define PC80_MC146818RTC_H

#include <arch/io.h>
#include <types.h>

#define RTC_BASE_PORT_BANK0 (CONFIG_PC_CMOS_BASE_PORT_BANK0)
#define RTC_BASE_PORT_BANK1 (CONFIG_PC_CMOS_BASE_PORT_BANK1)

#define RTC_PORT_BANK0(x)	(RTC_BASE_PORT_BANK0 + (x))

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
 * totaling to a max high interval of 2.228 ms.
 */
# define RTC_UIP		0x80
# define RTC_DIV_CTL		0x70
   /* divider control: refclock values 4.194 / 1.049 MHz / 32.768 kHz */
#  define RTC_REF_CLCK_4MHZ	0x00
#  define RTC_REF_CLCK_1MHZ	0x10
#  define RTC_REF_CLCK_32KHZ	0x20
   /* In AMD BKDG, bit 4 is DV0 bank selection. Bits 5 and 6 are reserved. */
#  define RTC_AMD_BANK_SELECT	0x10
   /* 2 values for divider stage reset, others for "testing purposes only" */
#  define RTC_DIV_RESET1	0x60
#  define RTC_DIV_RESET2	0x70
  /* Periodic intr. / Square wave rate select. 0 = none,
   * 1 = 32.8kHz,... 15 = 2Hz
   */
# define RTC_RATE_SELECT	0x0F
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

#define RTC_DATE_ALARM		RTC_REG_D
#define RTC_MONTH_ALARM		0

/* On PCs, the checksum is built only over bytes 16..45 */
#define PC_CKS_RANGE_START	16
#define PC_CKS_RANGE_END	45
#define PC_CKS_LOC		46

/* Tracking of fallback/normal boot. */
#define RTC_BOOT_BYTE		48
#define RTC_BOOT_NORMAL		0x1

static inline unsigned char cmos_read(unsigned char addr)
{
	int port = RTC_BASE_PORT_BANK0;
	if (addr >= 128) {
		port = RTC_BASE_PORT_BANK1;
		addr -= 128;
	}
	outb(addr, port + 0);
	return inb(port + 1);
}

static inline void cmos_write_inner(unsigned char val, unsigned char addr)
{
	int port = RTC_BASE_PORT_BANK0;
	if (addr >= 128) {
		port = RTC_BASE_PORT_BANK1;
		addr -= 128;
	}
	outb(addr, port + 0);
	outb(val, port + 1);
}

static inline u8 cmos_disable_rtc(void)
{
	u8 control_state = cmos_read(RTC_CONTROL);
	if (!(control_state & RTC_SET))
		cmos_write_inner(control_state | RTC_SET, RTC_CONTROL);
	return control_state;
}

static inline void cmos_restore_rtc(u8 control_state)
{
	if (!(control_state & RTC_SET))
		cmos_write_inner(control_state, RTC_CONTROL);
}

static inline void cmos_write(unsigned char val, unsigned char addr)
{
	u8 control_state;

	/*
	 * There are various places where RTC bits might be hiding,
	 * eg. the Century / AltCentury byte. So to be safe, disable
	 * RTC before changing any value.
	 */
	if (addr != RTC_CONTROL)
		control_state = cmos_disable_rtc();
	cmos_write_inner(val, addr);
	if (addr != RTC_CONTROL)
		cmos_restore_rtc(control_state);
}

static inline u32 cmos_read32(u8 offset)
{
	u32 value = 0;
	u8 i;
	for (i = 0; i < sizeof(value); ++i)
		value |= cmos_read(offset + i) << (i << 3);
	return value;
}

static inline void cmos_write32(u32 value, u8 offset)
{
	u8 i;
	for (i = 0; i < sizeof(value); ++i)
		cmos_write((value >> (i << 3)) & 0xff, offset + i);
}

void cmos_init(bool invalid);
void cmos_check_update_date(void);
int cmos_error(void);
int cmos_lb_cks_valid(void);

int cmos_checksum_valid(int range_start, int range_end, int cks_loc);
void cmos_set_checksum(int range_start, int range_end, int cks_loc);

#endif /*  PC80_MC146818RTC_H */
