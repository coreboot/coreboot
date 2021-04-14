/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * From U-Boot 2016.05
 */

#include <console/console.h>
#include <rtc.h>

#define FEBRUARY		2
#define STARTOFTIME		1970
#define SECDAY			86400L
#define SECYR			(SECDAY * 365)
#define LEAP_YEAR(year)		((year) % 4 == 0)
#define DAYS_IN_YEAR(a)		(LEAP_YEAR(a) ? 366 : 365)
#define DAYS_IN_MONTH(a)	(month_days[(a) - 1])

static const char *const weekdays[] = {
	"Sun", "Mon", "Tues", "Wednes", "Thurs", "Fri", "Satur"
};

/* Zeller's rule */
static int rtc_calc_weekday(struct rtc_time *tm)
{
	/* In Zeller's rule, January and February are treated as if they
	   are months 13 and 14 of the previous year (March is still month 3) */
	const int zyear = ((tm->mon < 3) ? tm->year - 1 : tm->year);
	const int q = tm->mday;
	const int m = (tm->mon < 3) ? tm->mon + 12 : tm->mon;
	const int K = zyear % 100;
	const int J = zyear / 100;

	/*
	 * Because of the way the modulo operator works with negative numbers,
	 * the traditional formulation of Zeller's rule must be modified
	 * slightly to make the numerator positive (i.e., add 5J instead of
	 * subtracting 2J).  Also subtract 1 so that Sunday is day 0.
	 */
	const int h = (q + (13 * (m + 1)) / 5
		       + K + (K / 4) + (J / 4) + (5 * J) - 1) % 7;

	tm->wday = h;
	return 0;
}

int rtc_to_tm(int tim, struct rtc_time *tm)
{
	int month_days[12] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	register int i;
	register long hms, day;

	day = tim / SECDAY;
	hms = tim % SECDAY;

	/* Hours, minutes, seconds are easy */
	tm->hour = hms / 3600;
	tm->min = (hms % 3600) / 60;
	tm->sec = (hms % 3600) % 60;

	/* Number of years in days */
	for (i = STARTOFTIME; day >= DAYS_IN_YEAR(i); i++)
		day -= DAYS_IN_YEAR(i);
	tm->year = i;

	/* Number of months in days left */
	if (LEAP_YEAR(tm->year))
		DAYS_IN_MONTH(FEBRUARY) = 29;
	for (i = 1; day >= DAYS_IN_MONTH(i); i++)
		day -= DAYS_IN_MONTH(i);
	DAYS_IN_MONTH(FEBRUARY) = 28;
	tm->mon = i;

	/* Days are what is left over (+1) from all that */
	tm->mday = day + 1;

	/* Determine the day of week */
	return rtc_calc_weekday(tm);
}

/*
 * Converts Gregorian date to seconds since 1970-01-01 00:00:00.
 * Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980,  mon=12,  day=31,  hour=23,  min=59,  sec=59.
 *
 * [For the Julian calendar (which was used in Russia before 1917,
 * Britain & colonies before 1752,  anywhere else before 1582,
 * and is still in use by some communities) leave out the
 * -year / 100 + year / 400 terms,  and add 10.]
 *
 * This algorithm was first published by Gauss (I think).
 *
 * WARNING: this function will overflow on 2106-02-07 06:28:16 on
 * machines where long is 32-bit! (However, as time_t is signed, we
 * will already get problems at other places on 2038-01-19 03:14:08)
 */
unsigned long rtc_mktime(const struct rtc_time *tm)
{
	int mon = tm->mon;
	int year = tm->year;
	int days, hours;

	mon -= 2;
	if (0 >= (int)mon) {	/* 1..12 -> 11, 12, 1..10 */
		mon += 12;	/* Puts Feb last since it has leap day */
		year -= 1;
	}

	days = (unsigned long)(year / 4 - year / 100 + year / 400 +
			       367 * mon / 12 + tm->mday) +
			       year * 365 - 719499;
	hours = days * 24 + tm->hour;
	return (hours * 60 + tm->min) * 60 + tm->sec;
}

void rtc_display(const struct rtc_time *tm)
{
	printk(BIOS_INFO, "Date: %5d-%02d-%02d (%sday)  Time: %2d:%02d:%02d\n",
	       tm->year,  tm->mon,  tm->mday,
	       (tm->wday < 0 || tm->wday > 6) ? "unknown " : weekdays[tm->wday],
	       tm->hour,  tm->min,  tm->sec);
}
