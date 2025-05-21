/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef LIBPAYLOAD_DELAY_H
#define LIBPAYLOAD_DELAY_H

#include <stdint.h>

#define NSECS_PER_SEC 1000000000
#define USECS_PER_SEC 1000000
#define MSECS_PER_SEC 1000
#define NSECS_PER_MSEC (NSECS_PER_SEC / MSECS_PER_SEC)
#define NSECS_PER_USEC (NSECS_PER_SEC / USECS_PER_SEC)
#define USECS_PER_MSEC (USECS_PER_SEC / MSECS_PER_SEC)

unsigned int get_cpu_speed(void);

/**
 * Delay for a specified number of nanoseconds.
 *
 * @param ns Number of nanoseconds to delay for.
 */
void ndelay(uint64_t n);

/**
 * Delay for a specified number of microseconds.
 *
 * @param us Number of microseconds to delay for.
 */
static inline void udelay(unsigned int us)
{
	ndelay((uint64_t)us * NSECS_PER_USEC);
}

/**
 * Delay for a specified number of milliseconds.
 *
 * @param ms Number of milliseconds to delay for.
 */
static inline void mdelay(unsigned int ms)
{
	ndelay((uint64_t)ms * NSECS_PER_MSEC);
}

/**
 * Delay for a specified number of seconds.
 *
 * @param s Number of seconds to delay for.
 */
static inline void delay(unsigned int s)
{
	ndelay((uint64_t)s * NSECS_PER_SEC);
}

#endif /* LIBPAYLOAD_DELAY_H */
