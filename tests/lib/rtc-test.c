/* SPDX-License-Identifier: GPL-2.0-only */

#include <inttypes.h>
#include <rtc.h>
#include <string.h>
#include <tests/test.h>

static void test_rtc_to_tm_from_unix_time(void **state)
{
	struct rtc_time tm;
	int tim;

	/* Zero-day */
	tim = 0;
	assert_int_equal(0, rtc_to_tm(tim, &tm));
	assert_int_equal(1970, tm.year);
	assert_int_equal(1, tm.mon);
	assert_int_equal(1, tm.mday);
	assert_int_equal(0, tm.hour);
	assert_int_equal(0, tm.min);
	assert_int_equal(0, tm.sec);
	assert_int_equal(4, tm.wday); /* Thursday */

	/* One second from time base */
	tim = 1;
	assert_int_equal(0, rtc_to_tm(tim, &tm));
	assert_int_equal(1970, tm.year);
	assert_int_equal(1, tm.mon);
	assert_int_equal(1, tm.mday);
	assert_int_equal(0, tm.hour);
	assert_int_equal(0, tm.min);
	assert_int_equal(1, tm.sec);
	assert_int_equal(4, tm.wday); /* Thursday */

	/* Full time value */
	tim = INT32_MAX;
	assert_int_equal(0, rtc_to_tm(tim, &tm));
	assert_int_equal(2038, tm.year);
	assert_int_equal(1, tm.mon);
	assert_int_equal(19, tm.mday);
	assert_int_equal(3, tm.hour);
	assert_int_equal(14, tm.min);
	assert_int_equal(7, tm.sec);
	assert_int_equal(2, tm.wday); /* Tuesday */

	/* Other common value */
	tim = 1618484725;
	assert_int_equal(0, rtc_to_tm(tim, &tm));
	assert_int_equal(2021, tm.year);
	assert_int_equal(4, tm.mon);
	assert_int_equal(15, tm.mday);
	assert_int_equal(11, tm.hour);
	assert_int_equal(5, tm.min);
	assert_int_equal(25, tm.sec);
	assert_int_equal(4, tm.wday); /* Thursday */

	/* Negative value - expect incorrect output */
	tim = -1;
	assert_int_equal(0, rtc_to_tm(tim, &tm));
	assert_int_equal(1970, tm.year);
	assert_int_equal(1, tm.mon);
	assert_int_equal(1, tm.mday);
	assert_int_equal(0, tm.hour);
	assert_int_equal(0, tm.min);
	assert_int_equal(-1, tm.sec);
	assert_int_equal(4, tm.wday); /* Thursday */
}

static void test_mktime(void **state)
{
	struct rtc_time tm;
	struct rtc_time tm2;
	memset(&tm, 0, sizeof(tm));
	memset(&tm2, 0, sizeof(tm2));

	/* Epoch start */
	tm = (struct rtc_time){
		.year = 1970, .mon = 1, .mday = 1, .hour = 0, .min = 0, .sec = 0,
	};
	assert_int_equal(0, rtc_mktime(&tm));

	/* Last correct value */
	tm = (struct rtc_time){
		.year = 2038, .mon = 1, .mday = 19, .hour = 3, .min = 14, .sec = 7,
	};
	assert_int_equal(INT32_MAX, rtc_mktime(&tm));

	/* Common non-leap year */
	tm = (struct rtc_time){
		.year = 1999, .mon = 12, .mday = 6, .hour = 16, .min = 13, .sec = 59,
	};
	assert_int_equal(944496839, rtc_mktime(&tm));

	/* Ensure that February 29 gives the same result as March 1 in non-leap year */
	tm = (struct rtc_time){
		.year = 2017, .mon = 2, .mday = 29, .hour = 1, .min = 2, .sec = 3,
	};
	tm2 = (struct rtc_time){
		.year = 2017, .mon = 3, .mday = 1, .hour = 1, .min = 2, .sec = 3,
	};
	assert_int_equal(rtc_mktime(&tm), rtc_mktime(&tm2));

	/* Leap year (only division by 4 rule applies) */
	tm = (struct rtc_time){
		.year = 2004, .mon = 8, .mday = 30, .hour = 13, .min = 45, .sec = 33,
	};
	assert_int_equal(1093873533, rtc_mktime(&tm));
	/* Last day of February in leap year */
	tm.mon = 2;
	tm.mday = 29;
	assert_int_equal(1078062333, rtc_mktime(&tm));
	/* Ensure that February 29 and March 1 have different and correct values
	   in leap year */
	tm = (struct rtc_time){
		.year = 2004, .mon = 3, .mday = 1, .hour = 7, .min = 7, .sec = 17,
	};
	tm2 = (struct rtc_time){
		.year = 2004, .mon = 2, .mday = 29, .hour = 7, .min = 7, .sec = 17,
	};
	/* There should be exactly one day of difference */
	assert_int_equal(24 * 60 * 60, rtc_mktime(&tm) - rtc_mktime(&tm2));

	/* Leap year (division by 400 rule applies and division by 100 is excluded) */
	tm = (struct rtc_time){
		.year = 2000, .mon = 6, .mday = 11, .hour = 21, .min = 3, .sec = 6,
	};
	assert_int_equal(960757386, rtc_mktime(&tm));
	tm.mon = 2;
	tm.mday = 29;
	assert_int_equal(951858186, rtc_mktime(&tm));

	tm = (struct rtc_time){
		.year = 2000, .mon = 3, .mday = 1, .hour = 10, .min = 55, .sec = 21,
	};
	tm2 = (struct rtc_time){
		.year = 2000, .mon = 2, .mday = 29, .hour = 10, .min = 55, .sec = 21,
	};
	assert_int_equal(24 * 60 * 60, rtc_mktime(&tm) - rtc_mktime(&tm2));
}

static void assert_rtc_time_equal(struct rtc_time *tm1, struct rtc_time *tm2)
{
	assert_int_equal(tm1->sec, tm2->sec);
	assert_int_equal(tm1->min, tm2->min);
	assert_int_equal(tm1->hour, tm2->hour);
	assert_int_equal(tm1->mday, tm2->mday);
	assert_int_equal(tm1->mon, tm2->mon);
	assert_int_equal(tm1->year, tm2->year);
	assert_int_equal(tm1->wday, tm2->wday);
}

/* This test check if combination of rtc_to_tm and rtc_mktime gives result equal to input.
   Week day is ignored by rtc_mktime, but is calculated by rtc_to_tm, so it is included
   in input. */
static void test_rtc_mktime_with_rtc_to_tm(void **state)
{
	struct rtc_time tm_in;
	struct rtc_time tm_out;
	int tim;

	memset(&tm_in, 0, sizeof(tm_in));
	memset(&tm_out, 0, sizeof(tm_out));

	/* Conversion from rtc_time to timestamp and back to rtc_time */
	tm_in = (struct rtc_time){
		.year = 1970, .mon = 1, .mday = 1, .hour = 0, .min = 0, .sec = 0, .wday = 4,
	};
	assert_int_equal(0, rtc_to_tm(rtc_mktime(&tm_in), &tm_out));
	assert_rtc_time_equal(&tm_in, &tm_out);

	tm_in = (struct rtc_time){
		.year = 2000, .mon = 2, .mday = 29, .hour = 13, .min = 4, .sec = 15, .wday = 2,
	};
	assert_int_equal(0, rtc_to_tm(rtc_mktime(&tm_in), &tm_out));
	assert_rtc_time_equal(&tm_in, &tm_out);

	tm_in = (struct rtc_time){
		.year = 2000, .mon = 3, .mday = 1, .hour = 13, .min = 8, .sec = 37, .wday = 3,
	};
	assert_int_equal(0, rtc_to_tm(rtc_mktime(&tm_in), &tm_out));
	assert_rtc_time_equal(&tm_in, &tm_out);

	tm_in = (struct rtc_time){
		.year = 2017, .mon = 12, .mday = 7, .hour = 8, .min = 18, .sec = 9, .wday = 4,
	};
	assert_int_equal(0, rtc_to_tm(rtc_mktime(&tm_in), &tm_out));
	assert_rtc_time_equal(&tm_in, &tm_out);

	tm_in = (struct rtc_time){
		.year = 2020, .mon = 2, .mday = 29, .hour = 18, .min = 50, .sec = 0, .wday = 6,
	};
	assert_int_equal(0, rtc_to_tm(rtc_mktime(&tm_in), &tm_out));
	assert_rtc_time_equal(&tm_in, &tm_out);

	tm_in = (struct rtc_time){
		.year = 2020, .mon = 3, .mday = 1, .hour = 1, .min = 20, .sec = 23, .wday = 0,
	};
	assert_int_equal(0, rtc_to_tm(rtc_mktime(&tm_in), &tm_out));
	assert_rtc_time_equal(&tm_in, &tm_out);


	/* Conversion from timestamp to rtc_time and back to timestamp */
	tim = 0;
	rtc_to_tm(tim, &tm_out);
	assert_int_equal(tim, rtc_mktime(&tm_out));

	tim = INT32_MAX;
	rtc_to_tm(tim, &tm_out);
	assert_int_equal(tim, rtc_mktime(&tm_out));

	/* 2000-02-29 1:23:34 */
	tim = 951787414;
	rtc_to_tm(tim, &tm_out);
	assert_int_equal(tim, rtc_mktime(&tm_out));

	/* 2000-03-01 1:23:34 */
	tim = 951873814;
	rtc_to_tm(tim, &tm_out);
	assert_int_equal(tim, rtc_mktime(&tm_out));

	/* 1999-09-09 9:09:09 */
	tim = 936868149;
	rtc_to_tm(tim, &tm_out);
	assert_int_equal(tim, rtc_mktime(&tm_out));

	/* 2020-02-29 2:29:02 */
	tim = 1582943342;
	rtc_to_tm(tim, &tm_out);
	assert_int_equal(tim, rtc_mktime(&tm_out));

	/* 2020-03-01 3:01:03 */
	tim = 1583031663;
	rtc_to_tm(tim, &tm_out);
	assert_int_equal(tim, rtc_mktime(&tm_out));
}

static void test_leap_day_secday(void **state)
{
	const int secday = 60 * 60 * 24;
	struct rtc_time tm_in;
	struct rtc_time tm_out;
	struct rtc_time tm_expected;
	int tim;

	memset(&tm_in, 0, sizeof(tm_in));
	memset(&tm_out, 0, sizeof(tm_out));

	/* Non-leap year */
	tm_in = (struct rtc_time){
		.year = 1999, .mon = 2, .mday = 28, .hour = 5, .min = 37, .sec = 15, .wday = 0,
	};
	tim = rtc_mktime(&tm_in) + secday;
	tm_expected = (struct rtc_time){
		.year = 1999, .mon = 3, .mday = 1, .hour = 5, .min = 37, .sec = 15, .wday = 1,
	};
	assert_int_equal(0, rtc_to_tm(tim, &tm_out));
	assert_rtc_time_equal(&tm_out, &tm_expected);

	/* Leap-year February 28 to February 29 */
	tm_in = (struct rtc_time){
		.year = 2000, .mon = 2, .mday = 28, .hour = 0, .min = 33, .sec = 11, .wday = 1,
	};
	tim = rtc_mktime(&tm_in) + secday;
	tm_expected = (struct rtc_time){
		.year = 2000, .mon = 2, .mday = 29, .hour = 0, .min = 33, .sec = 11, .wday = 2,
	};
	assert_int_equal(0, rtc_to_tm(tim, &tm_out));
	assert_rtc_time_equal(&tm_out, &tm_expected);

	tm_in = (struct rtc_time){
		.year = 2004, .mon = 2, .mday = 28, .hour = 9, .min = 13, .sec = 45, .wday = 6,
	};
	tim = rtc_mktime(&tm_in) + secday;
	tm_expected = (struct rtc_time){
		.year = 2004, .mon = 2, .mday = 29, .hour = 9, .min = 13, .sec = 45, .wday = 0,
	};
	assert_int_equal(0, rtc_to_tm(tim, &tm_out));
	assert_rtc_time_equal(&tm_out, &tm_expected);

	/* Leap-year February 29 to March 1 */
	tm_in = (struct rtc_time){
		.year = 2000, .mon = 2, .mday = 29, .hour = 22, .min = 50, .sec = 25, .wday = 2,
	};
	tim = rtc_mktime(&tm_in) + secday;
	tm_expected = (struct rtc_time){
		.year = 2000, .mon = 3, .mday = 1, .hour = 22, .min = 50, .sec = 25, .wday = 3,
	};
	assert_int_equal(0, rtc_to_tm(tim, &tm_out));
	assert_rtc_time_equal(&tm_out, &tm_expected);

	tm_in = (struct rtc_time){
		.year = 2004, .mon = 2, .mday = 29, .hour = 17, .min = 56, .sec = 27, .wday = 0,
	};
	tim = rtc_mktime(&tm_in) + secday;
	tm_expected = (struct rtc_time){
		.year = 2004, .mon = 3, .mday = 1, .hour = 17, .min = 56, .sec = 27, .wday = 1,
	};
	assert_int_equal(0, rtc_to_tm(tim, &tm_out));
	assert_rtc_time_equal(&tm_out, &tm_expected);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_rtc_to_tm_from_unix_time),
		cmocka_unit_test(test_mktime),
		cmocka_unit_test(test_rtc_mktime_with_rtc_to_tm),
		cmocka_unit_test(test_leap_day_secday),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
