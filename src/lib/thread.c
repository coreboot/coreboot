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
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <arch/cpu.h>
#include <bootstate.h>
#include <console/console.h>
#include <thread.h>

static void idle_thread_init(void);

/* There needs to be at least one thread to run the ramstate state machine. */
#define TOTAL_NUM_THREADS (CONFIG_NUM_THREADS + 1)
extern char thread_stacks[CONFIG_NUM_THREADS*CONFIG_STACK_SIZE];

/* Storage space for the thread structs .*/
static struct thread all_threads[TOTAL_NUM_THREADS];

/* All runnable (but not running) and free threads are kept on their
 * respective lists. */
static struct thread *runnable_threads;
static struct thread *free_threads;

static inline struct cpu_info *thread_cpu_info(const struct thread *t)
{
	return (void *)(t->stack_orig);
}

static inline int thread_can_yield(const struct thread *t)
{
	return (t != NULL && t->can_yield);
}

/* Assumes current cpu info can switch. */
static inline struct thread *cpu_info_to_thread(const struct cpu_info *ci)
{
	return ci->thread;
}

static inline struct thread *current_thread(void)
{
	return cpu_info_to_thread(cpu_info());
}

static inline int thread_list_empty(struct thread **list)
{
	return *list == NULL;
}

static inline struct thread *pop_thread(struct thread **list)
{
	struct thread *t;

	t = *list;
	*list = t->next;
	t->next = NULL;
	return t;
}

static inline void push_thread(struct thread **list, struct thread *t)
{
	t->next = *list;
	*list = t;
}

static inline void push_runnable(struct thread *t)
{
	push_thread(&runnable_threads, t);
}

static inline struct thread *pop_runnable(void)
{
	return pop_thread(&runnable_threads);
}

static inline struct thread *get_free_thread(void)
{
	struct thread *t;
	struct cpu_info *ci;
	struct cpu_info *new_ci;

	if (thread_list_empty(&free_threads))
		return NULL;

	t = pop_thread(&free_threads);

	ci = cpu_info();

	/* Initialize the cpu_info structure on the new stack. */
	new_ci = thread_cpu_info(t);
	*new_ci = *ci;
	new_ci->thread = t;

	/* Reset the current stack value to the original. */
	t->stack_current = t->stack_orig;

	return t;
}

static inline void free_thread(struct thread *t)
{
	push_thread(&free_threads, t);
}

/* The idle thread is ran whenever there isn't anything else that is runnable.
 * It's sole responsibility is to ensure progress is made by running the timer
 * callbacks. */
static void idle_thread(void *unused)
{
	/* This thread never voluntarily yields. */
	thread_prevent_coop();
	while (1) {
		timers_run();
	}
}

static void schedule(struct thread *t)
{
	struct thread *current = current_thread();

	/* If t is NULL need to find new runnable thread. */
	if (t == NULL) {
		if (thread_list_empty(&runnable_threads))
			die("Runnable thread list is empty!\n");
		t = pop_runnable();
	} else {
		/* current is still runnable. */
		push_runnable(current);
	}
	switch_to_thread(t->stack_current, &current->stack_current);
}

static void terminate_thread(struct thread *t)
{
	free_thread(t);
	schedule(NULL);
}

static void asmlinkage call_wrapper(void *unused)
{
	struct thread *current = current_thread();

	current->entry(current->entry_arg);
	terminate_thread(current);
}

/* Block the current state transitions until thread is complete. */
static void asmlinkage call_wrapper_block_current(void *unused)
{
	struct thread *current = current_thread();

	boot_state_current_block();
	current->entry(current->entry_arg);
	boot_state_current_unblock();
	terminate_thread(current);
}

struct block_boot_state {
	boot_state_t state;
	boot_state_sequence_t seq;
};

/* Block the provided state until thread is complete. */
static void asmlinkage call_wrapper_block_state(void *arg)
{
	struct block_boot_state *bbs = arg;
	struct thread *current = current_thread();

	boot_state_block(bbs->state, bbs->seq);
	current->entry(current->entry_arg);
	boot_state_unblock(bbs->state, bbs->seq);
	terminate_thread(current);
}

/* Prepare a thread so that it starts by executing thread_entry(thread_arg).
 * Within thread_entry() it will call func(arg). */
static void prepare_thread(struct thread *t, void *func, void *arg,
                           void asmlinkage (*thread_entry)(void *),
                           void *thread_arg)
{
	/* Stash the function and argument to run. */
	t->entry = func;
	t->entry_arg = arg;

	/* All new threads can yield by default. */
	t->can_yield = 1;

	arch_prepare_thread(t, thread_entry, thread_arg);
}

static void thread_resume_from_timeout(struct timeout_callback *tocb)
{
	struct thread *to;

	to = tocb->priv;
	schedule(to);
}

static void idle_thread_init(void)
{
	struct thread *t;

	t = get_free_thread();

	if (t == NULL) {
		die("No threads available for idle thread!\n");
	}

	/* Queue idle thread to run once all other threads have yielded. */
	prepare_thread(t, idle_thread, NULL, call_wrapper, NULL);
	push_runnable(t);
	/* Mark the currently executing thread to cooperate. */
	thread_cooperate();
}

/* Don't inline this function so the timeout_callback won't have its storage
 * space on the stack cleaned up before the call to schedule(). */
static int __attribute__((noinline))
thread_yield_timed_callback(struct timeout_callback *tocb, unsigned microsecs)
{
	tocb->priv = current_thread();
	tocb->callback = thread_resume_from_timeout;

	if (timer_sched_callback(tocb, microsecs))
		return -1;

	/* The timer callback will wake up the current thread. */
	schedule(NULL);
	return 0;
}

static void *thread_alloc_space(struct thread *t, size_t bytes)
{
	/* Allocate the amount of space on the stack keeping the stack
	 * aligned to the pointer size. */
	t->stack_current -= ALIGN_UP(bytes, sizeof(uintptr_t));

	return (void *)t->stack_current;
}

void threads_initialize(void)
{
	int i;
	struct thread *t;
	char *stack_top;
	struct cpu_info *ci;

	/* Initialize the BSP thread first. The cpu_info structure is assumed
	 * to be just under the top of the stack. */
	t = &all_threads[0];
	ci = cpu_info();
	ci->thread = t;
	t->stack_orig = (uintptr_t)ci;
	t->id = 0;

	stack_top = &thread_stacks[CONFIG_STACK_SIZE] - sizeof(struct cpu_info);
	for (i = 1; i < TOTAL_NUM_THREADS; i++) {
		t = &all_threads[i];
		t->stack_orig = (uintptr_t)stack_top;
		t->id = i;
		stack_top += CONFIG_STACK_SIZE;
		free_thread(t);
	}

	idle_thread_init();
}

int thread_run(void (*func)(void *), void *arg)
{
	struct thread *current;
	struct thread *t;

	current = current_thread();

	if (!thread_can_yield(current)) {
		printk(BIOS_ERR,
		       "thread_run() called from non-yielding context!\n");
		return -1;
	}

	t = get_free_thread();

	if (t == NULL) {
		printk(BIOS_ERR, "thread_run() No more threads!\n");
		return -1;
	}

	prepare_thread(t, func, arg, call_wrapper_block_current, NULL);
	schedule(t);

	return 0;
}

int thread_run_until(void (*func)(void *), void *arg,
                     boot_state_t state, boot_state_sequence_t seq)
{
	struct thread *current;
	struct thread *t;
	struct block_boot_state *bbs;

	current = current_thread();

	if (!thread_can_yield(current)) {
		printk(BIOS_ERR,
		       "thread_run() called from non-yielding context!\n");
		return -1;
	}

	t = get_free_thread();

	if (t == NULL) {
		printk(BIOS_ERR, "thread_run() No more threads!\n");
		return -1;
	}

	bbs = thread_alloc_space(t, sizeof(*bbs));
	bbs->state = state;
	bbs->seq = seq;
	prepare_thread(t, func, arg, call_wrapper_block_state, bbs);
	schedule(t);

	return 0;
}

int thread_yield_microseconds(unsigned microsecs)
{
	struct thread *current;
	struct timeout_callback tocb;

	current = current_thread();

	if (!thread_can_yield(current))
		return -1;

	if (thread_yield_timed_callback(&tocb, microsecs))
		return -1;

	return 0;
}

void thread_cooperate(void)
{
	struct thread *current;

	current = current_thread();

	if (current != NULL)
		current->can_yield = 1;
}

void thread_prevent_coop(void)
{
	struct thread *current;

	current = current_thread();

	if (current != NULL)
		current->can_yield = 0;
}
