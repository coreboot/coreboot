/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Helper functions for rational numbers.
 *
 * Copyright (C) 2009 emlix GmbH, Oskar Schirmer <oskar@scara.com>
 * Copyright (C) 2019 Trent Piepho <tpiepho@gmail.com>
 */

#include <commonlib/helpers.h>
#include <commonlib/rational.h>
#include <limits.h>

/*
 * For theoretical background, see:
 * https://en.wikipedia.org/wiki/Continued_fraction
 */
void rational_best_approximation(
	unsigned long numerator, unsigned long denominator,
	unsigned long max_numerator, unsigned long max_denominator,
	unsigned long *best_numerator, unsigned long *best_denominator)
{
	/*
	 * n/d is the starting rational, where both n and d will
	 * decrease in each iteration using the Euclidean algorithm.
	 *
	 * dp is the value of d from the prior iteration.
	 *
	 * n2/d2, n1/d1, and n0/d0 are our successively more accurate
	 * approximations of the rational.  They are, respectively,
	 * the current, previous, and two prior iterations of it.
	 *
	 * a is current term of the continued fraction.
	 */
	unsigned long n, d, n0, d0, n1, d1, n2, d2;
	n = numerator;
	d = denominator;
	n0 = d1 = 0;
	n1 = d0 = 1;

	for (;;) {
		unsigned long dp, a;

		if (d == 0)
			break;
		/*
		 * Find next term in continued fraction, 'a', via
		 * Euclidean algorithm.
		 */
		dp = d;
		a = n / d;
		d = n % d;
		n = dp;

		/*
		 * Calculate the current rational approximation (aka
		 * convergent), n2/d2, using the term just found and
		 * the two prior approximations.
		 */
		n2 = n0 + a * n1;
		d2 = d0 + a * d1;

		/*
		 * If the current convergent exceeds the maximum, then
		 * return either the previous convergent or the
		 * largest semi-convergent, the final term of which is
		 * found below as 't'.
		 */
		if ((n2 > max_numerator) || (d2 > max_denominator)) {
			unsigned long t = ULONG_MAX;

			if (d1)
				t = (max_denominator - d0) / d1;
			if (n1)
				t = MIN(t, (max_numerator - n0) / n1);

			/*
			 * This tests if the semi-convergent is closer than the previous
			 * convergent.  If d1 is zero there is no previous convergent as
			 * this is the 1st iteration, so always choose the semi-convergent.
			 */
			if (!d1 || 2u * t > a || (2u * t == a && d0 * dp > d1 * d)) {
				n1 = n0 + t * n1;
				d1 = d0 + t * d1;
			}
			break;
		}
		n0 = n1;
		n1 = n2;
		d0 = d1;
		d1 = d2;
	}

	*best_numerator = n1;
	*best_denominator = d1;
}
