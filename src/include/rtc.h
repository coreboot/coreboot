/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _RTC_H_
#define _RTC_H_

struct rtc_time {
	int sec;
	int min;
	int hour;
	int mday;
	int mon;
	int year;
	int wday;
};

/* Implemented by the RTC driver (there can be only one) */
int rtc_set(const struct rtc_time *time);
int rtc_get(struct rtc_time *time);

/* Common functions */
int rtc_to_tm(int tim, struct rtc_time *tm);
unsigned long rtc_mktime(const struct rtc_time *tm);
void rtc_display(const struct rtc_time *tm);

#endif /* _RTC_H_ */
