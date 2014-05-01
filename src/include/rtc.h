/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google, Inc.
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
 */

#ifndef _RTC_H_
#define _RTC_H_

struct rtc_time
{
	int sec;
	int min;
	int hour;
	int mday;
	int mon;
	int year;
	int wday;
};

int rtc_set(const struct rtc_time *time);
int rtc_get(struct rtc_time *time);

#endif /* _RTC_H_ */
