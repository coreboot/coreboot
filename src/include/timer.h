/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef TIMER_H
#define TIMER_H

#include <types.h>

#define NSECS_PER_SEC 1000000000
#define USECS_PER_SEC 1000000
#define MSECS_PER_SEC 1000
#define USECS_PER_MSEC (USECS_PER_SEC / MSECS_PER_SEC)

/* The time structures are defined to be a representation of the time since
 * coreboot started executing one of its stages. The reason for using structures
 * is to allow for changes in the future. The structures' details are exposed
 * so that the compiler can allocate space on the stack and use in other
 * structures. In other words, accessing any field within this structure
 * outside of the core timer code is not supported. */

struct mono_time {
	uint64_t microseconds;
};

/* A timeout_callback structure is used for the book keeping for scheduling
 * work in the future. When a callback is called the structure can be
 * re-used for scheduling as it is not being tracked by the core timer
 * library any more. */
struct timeout_callback {
	void *priv;
	void (*callback)(struct timeout_callback *tocb);
	/* Not for public use. The timer library uses the fields below. */
	struct mono_time expiration;
};

/* Obtain the current monotonic time. The assumption is that the time counts
 * up from the value 0 with value 0 being the point when the timer was
 * initialized.  Additionally, the timer is assumed to only be valid for the
 * duration of the boot.
 *
 * Note that any implementations of timer_monotonic_get()
 * need to ensure its timesource does not roll over within 10 secs. The reason
 * is that the time between calls to timer_monotonic_get() may be on order
 * of 10 seconds. */
void timer_monotonic_get(struct mono_time *mt);

/* Returns 1 if callbacks still present in the queue. 0 if no timers left. */
int timers_run(void);

/* Schedule a callback to be ran microseconds from time of invocation.
 * 0 returned on success, < 0 on error. */
int timer_sched_callback(struct timeout_callback *tocb, unsigned long us);

/* Set an absolute time to a number of microseconds. */
static inline void mono_time_set_usecs(struct mono_time *mt, long us)
{
	mt->microseconds = us;
}

/* Set an absolute time to a number of milliseconds. */
static inline void mono_time_set_msecs(struct mono_time *mt, long ms)
{
	mt->microseconds = ms * USECS_PER_MSEC;
}

/* Add microseconds to an absolute time. */
static inline void mono_time_add_usecs(struct mono_time *mt, long us)
{
	mt->microseconds += us;
}

/* Add milliseconds to an absolute time. */
static inline void mono_time_add_msecs(struct mono_time *mt, long ms)
{
	mono_time_add_usecs(mt, ms * USECS_PER_MSEC);
}

/* Compare two absolute times: Return -1, 0, or 1 if t1 is <, =, or > t2,
 * respectively. */
static inline int mono_time_cmp(const struct mono_time *t1,
				const struct mono_time *t2)
{
	if (t1->microseconds == t2->microseconds)
		return 0;

	if (t1->microseconds < t2->microseconds)
		return -1;

	return 1;
}

/* Return true if t1 after t2  */
static inline int mono_time_after(const struct mono_time *t1,
				  const struct mono_time *t2)
{
	return mono_time_cmp(t1, t2) > 0;
}

/* Return true if t1 before t2. */
static inline int mono_time_before(const struct mono_time *t1,
				   const struct mono_time *t2)
{
	return mono_time_cmp(t1, t2) < 0;
}

/* Return time difference between t1 and t2. i.e. t2 - t1. */
static inline long mono_time_diff_microseconds(const struct mono_time *t1,
					       const struct mono_time *t2)
{
	return t2->microseconds - t1->microseconds;
}

struct stopwatch {
	struct mono_time start;
	struct mono_time current;
	struct mono_time expires;
};

static inline void stopwatch_init(struct stopwatch *sw)
{
	if (CONFIG(HAVE_MONOTONIC_TIMER))
		timer_monotonic_get(&sw->start);
	else
		sw->start.microseconds = 0;

	sw->current = sw->expires = sw->start;
}

static inline void stopwatch_init_usecs_expire(struct stopwatch *sw, long us)
{
	stopwatch_init(sw);
	mono_time_add_usecs(&sw->expires, us);
}

static inline void stopwatch_init_msecs_expire(struct stopwatch *sw, long ms)
{
	stopwatch_init_usecs_expire(sw, USECS_PER_MSEC * ms);
}

/*
 * Tick the stopwatch to collect the current time.
 */
static inline void stopwatch_tick(struct stopwatch *sw)
{
	if (CONFIG(HAVE_MONOTONIC_TIMER))
		timer_monotonic_get(&sw->current);
	else
		sw->current.microseconds = 0;
}

/*
 * Tick and check the stopwatch for expiration. Returns non-zero on expiration.
 */
static inline int stopwatch_expired(struct stopwatch *sw)
{
	stopwatch_tick(sw);
	return !mono_time_before(&sw->current, &sw->expires);
}

/*
 * Tick and check the stopwatch as long as it has not expired.
 */
static inline void stopwatch_wait_until_expired(struct stopwatch *sw)
{
	while (!stopwatch_expired(sw))
		;
}

/*
 * Return number of microseconds since starting the stopwatch.
 */
static inline long stopwatch_duration_usecs(struct stopwatch *sw)
{
	/*
	 * If the stopwatch hasn't been ticked (current == start) tick
	 * the stopwatch to gather the accumulated time.
	 */
	if (!mono_time_cmp(&sw->start, &sw->current))
		stopwatch_tick(sw);

	return mono_time_diff_microseconds(&sw->start, &sw->current);
}

static inline long stopwatch_duration_msecs(struct stopwatch *sw)
{
	return stopwatch_duration_usecs(sw) / USECS_PER_MSEC;
}

/*
 * Helper macro to wait until a condition becomes true or a timeout elapses.
 *
 * condition: a C expression to wait for
 * timeout: timeout, in microseconds
 *
 * Returns:
 *  0   if the condition still evaluates to false after the timeout elapsed,
 * >0   if the condition evaluates to true. The return value is the amount of
 *      microseconds waited (at least 1).
 */
#define wait_us(timeout_us, condition)					\
({									\
	long __ret = 0;							\
	struct stopwatch __sw;						\
	stopwatch_init_usecs_expire(&__sw, timeout_us);			\
	do {								\
		if (condition) {					\
			stopwatch_tick(&__sw);				\
			__ret = stopwatch_duration_usecs(&__sw);	\
			if (!__ret) /* make sure it evaluates to true */\
				__ret = 1;				\
			break;						\
		}							\
	} while (!stopwatch_expired(&__sw));				\
	__ret;								\
})

#define wait_ms(timeout_ms, condition)					\
	DIV_ROUND_UP(wait_us((timeout_ms) * USECS_PER_MSEC, condition), \
		     USECS_PER_MSEC)

#endif /* TIMER_H */
