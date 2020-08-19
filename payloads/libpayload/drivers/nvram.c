/*
 *
 * Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2017 Patrick Rudolph <siro@das-labor.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Datasheet:
 *  - Name: MC146818: Real-time Clock Plus RAM (RTC)
 *  - PDF: http://www.freescale.com/files/microcontrollers/doc/data_sheet/MC146818.pdf
 *  - Order number: MC146818/D
 */

/*
 * See also:
 * http://bochs.sourceforge.net/techspec/CMOS-reference.txt
 * http://www.bioscentral.com/misc/cmosmap.htm
 */
#include <libpayload-config.h>
#include <libpayload.h>

/**
 * PCs can have either 64 (very old ones), 128, or 256 bytes of CMOS RAM.
 *
 * Usually you access the lower 128 CMOS bytes via I/O port 0x70/0x71.
 * For more recent chipsets with 256 bytes, you have to access the upper
 * 128 bytes (128-255) using two different registers, usually 0x72/0x73.
 *
 * On some chipsets this can be different, though. The VIA VT8237R for example
 * only recognizes the ports 0x74/0x75 for accessing the high 128 CMOS bytes
 * (as seems to be the case for multiple VIA chipsets).
 *
 * It's very chipset-specific if and how the upper 128 bytes are enabled at
 * all, but this work should be done in coreboot anyway. Libpayload assumes
 * that coreboot has properly enabled access to the upper 128 bytes and
 * doesn't try to do this on its own.
 */
#define RTC_PORT_STANDARD      0x70
#if CONFIG(LP_RTC_PORT_EXTENDED_VIA)
#define RTC_PORT_EXTENDED      0x74
#else
#define RTC_PORT_EXTENDED      0x72
#endif

/**
 * Read a byte from the specified NVRAM address.
 *
 * @param addr The NVRAM address to read a byte from.
 * @return The byte at the given NVRAM address.
 */
u8 nvram_read(u8 addr)
{
	u16 rtc_port = addr < 128 ? RTC_PORT_STANDARD : RTC_PORT_EXTENDED;

	outb(addr, rtc_port);
	return inb(rtc_port + 1);
}

/**
 * Write a byte to the specified NVRAM address.
 *
 * @param val The byte to write to NVRAM.
 * @param addr The NVRAM address to write to.
 */
void nvram_write(u8 val, u8 addr)
{
	u16 rtc_port = addr < 128 ? RTC_PORT_STANDARD : RTC_PORT_EXTENDED;

	outb(addr, rtc_port);
	outb(val, rtc_port + 1);
}

/**
 * Return 1 if the NVRAM is currently updating and a 0 otherwise
 * @return A 1 if the NVRAM is updating and 0 otherwise
 */

int nvram_updating(void)
{
       return (nvram_read(NVRAM_RTC_FREQ_SELECT) & NVRAM_RTC_UIP) ? 1 : 0;
}

/**
 * Get the current time and date from the RTC
 *
 * @param time A pointer to a broken-down time structure
 */
void rtc_read_clock(struct tm *time)
{
	u16 timeout = 10000;
	u8 statusB;
	u8 reg8;

	memset(time, 0, sizeof(*time));

	while (nvram_updating())
		if (!timeout--)
			return;

	statusB = nvram_read(NVRAM_RTC_STATUSB);

	if (!(statusB & NVRAM_RTC_FORMAT_BINARY)) {
		time->tm_mon = bcd2dec(nvram_read(NVRAM_RTC_MONTH)) - 1;
		time->tm_sec = bcd2dec(nvram_read(NVRAM_RTC_SECONDS));
		time->tm_min = bcd2dec(nvram_read(NVRAM_RTC_MINUTES));
		time->tm_mday = bcd2dec(nvram_read(NVRAM_RTC_DAY));

		if (!(statusB & NVRAM_RTC_FORMAT_24HOUR)) {
			reg8 = nvram_read(NVRAM_RTC_HOURS);
			time->tm_hour = bcd2dec(reg8 & 0x7f);
			time->tm_hour += (reg8 & 0x80) ? 12 : 0;
			time->tm_hour %= 24;
		} else {
			time->tm_hour = bcd2dec(nvram_read(NVRAM_RTC_HOURS));
		}
		time->tm_year = bcd2dec(nvram_read(NVRAM_RTC_YEAR));
	} else {
		time->tm_mon = nvram_read(NVRAM_RTC_MONTH) - 1;
		time->tm_sec = nvram_read(NVRAM_RTC_SECONDS);
		time->tm_min = nvram_read(NVRAM_RTC_MINUTES);
		time->tm_mday = nvram_read(NVRAM_RTC_DAY);
		if (!(statusB & NVRAM_RTC_FORMAT_24HOUR)) {
			reg8 = nvram_read(NVRAM_RTC_HOURS);
			time->tm_hour = reg8 & 0x7f;
			time->tm_hour += (reg8 & 0x80) ? 12 : 0;
			time->tm_hour %= 24;
		} else {
			time->tm_hour = nvram_read(NVRAM_RTC_HOURS);
		}
		time->tm_year = nvram_read(NVRAM_RTC_YEAR);
	}

	/* Instead of finding the century register,
	   we just make an assumption that if the year value is
	   less then 80, then it is 2000+
	*/
	if (time->tm_year < 80)
		time->tm_year += 100;
}

/**
 * Write the current time and date to the RTC
 *
 * @param time A pointer to a broken-down time structure
 */
void rtc_write_clock(const struct tm *time)
{
	u16 timeout = 10000;
	u8 statusB;
	u8 reg8, year;

	while (nvram_updating())
		if (!timeout--)
			return;

	statusB = nvram_read(NVRAM_RTC_STATUSB);

	year = time->tm_year;
	if (year > 100)
		year -= 100;

	if (!(statusB & NVRAM_RTC_FORMAT_BINARY)) {
		nvram_write(dec2bcd(time->tm_mon + 1), NVRAM_RTC_MONTH);
		nvram_write(dec2bcd(time->tm_sec), NVRAM_RTC_SECONDS);
		nvram_write(dec2bcd(time->tm_min), NVRAM_RTC_MINUTES);
		nvram_write(dec2bcd(time->tm_mday), NVRAM_RTC_DAY);
		if (!(statusB & NVRAM_RTC_FORMAT_24HOUR)) {
			if (time->tm_hour > 12)
				reg8 = dec2bcd(time->tm_hour - 12) | 0x80;
			else
				reg8 = dec2bcd(time->tm_hour);
		} else {
			reg8 = dec2bcd(time->tm_hour);
		}
		nvram_write(reg8, NVRAM_RTC_HOURS);
		nvram_write(dec2bcd(year), NVRAM_RTC_YEAR);
	} else {
		nvram_write(time->tm_mon + 1, NVRAM_RTC_MONTH);
		nvram_write(time->tm_sec, NVRAM_RTC_SECONDS);
		nvram_write(time->tm_min, NVRAM_RTC_MINUTES);
		nvram_write(time->tm_mday, NVRAM_RTC_DAY);
		if (!(statusB & NVRAM_RTC_FORMAT_24HOUR)) {
			if (time->tm_hour > 12)
				reg8 = (time->tm_hour - 12) | 0x80;
			else
				reg8 = time->tm_hour;
		} else {
			reg8 = time->tm_hour;
		}
		nvram_write(reg8, NVRAM_RTC_HOURS);
		nvram_write(year, NVRAM_RTC_YEAR);
	}
}
