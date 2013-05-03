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
#ifndef THREAD_H_
#define THREAD_H_

#include <stddef.h>
#include <timer.h>
#include <arch/cpu.h>

#if CONFIG_COOP_MULTITASKING && !defined(__SMM__) && !defined(__PRE_RAM__)
void threads_initialize(void);
/* Return 0 on success, < 0 on failure. */
int thread_run(void (*func)(void *), void *arg);
/* Return 0 on success, < 0 on failure. */
int thread_yield_for(unsigned microsecs);
/* thread_resume() is exposed only to be used by the timer code. */
void thread_resume(struct timeout_callback *tocb);

static inline int tocb_is_resume(const struct timeout_callback *tocb)
{
	return tocb->callback == thread_resume;
}

void thread_cooperate(void);
void thread_prevent_coop(void);

static inline void thread_init_cpu_info_non_bsp(struct cpu_info *ci)
{
	ci->thread = NULL;
}
#else
static inline void threads_initialize(void) {}
static inline int thread_run(void (*func)(void *), void *arg) { return -1; }
static inline int thread_yield_for(unsigned microsecs) { return -1; }
static inline int tocb_is_resume(const struct timeout_callback *tocb)
{
	return 0;
}
static inline void thread_cooperate(void) {}
static inline void thread_prevent_coop(void) {}
static inline void thread_init_cpu_info_non_bsp(struct cpu_info *ci) { }
#endif

#endif /* THREAD_H_ */
