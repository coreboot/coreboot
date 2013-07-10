/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef TIMER_H
#define TIMER_H

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
	long microseconds;
};

struct rela_time {
	long microseconds;
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

static inline void mono_time_add_rela_time(struct mono_time *mt,
                                           const struct rela_time *t)
{
	mono_time_add_usecs(mt, t->microseconds);
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

static inline int rela_time_cmp(const struct rela_time *t1,
                                const struct rela_time *t2)
{
	if (t1->microseconds == t2->microseconds)
		return 0;

	if (t1->microseconds < t2->microseconds)
		return -1;

	return 1;
}

/* Initialize a rela_time structure. */
static inline struct rela_time rela_time_init_usecs(long us)
{
	struct rela_time t;
	t.microseconds = us;
	return t;
}

/* Return time difference between t1 and t2. i.e. t2 - t1. */
static struct rela_time mono_time_diff(const struct mono_time *t1,
                                       const struct mono_time *t2)
{
	return rela_time_init_usecs(t2->microseconds - t1->microseconds);
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

/* Return the difference between now and t. */
static inline struct rela_time current_time_from(const struct mono_time *t)
{
	struct mono_time now;

	timer_monotonic_get(&now);
	return mono_time_diff(t, &now);

}

static inline long rela_time_in_microseconds(const struct rela_time *rt)
{
	return rt->microseconds;
}

static inline long mono_time_diff_microseconds(const struct mono_time *t1,
					       const struct mono_time *t2)
{
	struct rela_time rt;
	rt = mono_time_diff(t1, t2);
	return rela_time_in_microseconds(&rt);
}

#endif /* TIMER_H */
