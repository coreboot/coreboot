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
#include <arch/cpu.h>
#include <console/console.h>
#include <thread.h>

#define TOTAL_NUM_THREADS (CONFIG_NUM_THREADS + 1)
extern void asmlinkage switch_to_thread(uintptr_t new_stack,
                                        uintptr_t *saved_stack);
extern char thread_stacks[CONFIG_NUM_THREADS*CONFIG_STACK_SIZE];

struct thread {
	int id;
	uintptr_t saved_stack;
	char *stack_top;
	struct thread *next;
	struct timeout_callback tocb;
	void (*entry)(void *);
	void *entry_arg;
	int can_yield;
};

struct pushad_regs {
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
};

static struct thread *runnable_threads;
static struct thread *free_threads;
static struct thread all_threads[TOTAL_NUM_THREADS];

static inline struct cpu_info *thread_cpu_info(const struct thread *t)
{
	return (void *)(t->stack_top - sizeof(struct cpu_info));
}

static inline int cpu_info_can_switch(const struct cpu_info *ci)
{
	return (ci->thread != NULL);
}

static inline int thread_is_main(const struct thread *t)
{
	return t->id == 0;
}

/* Assumes current cpu info can switch. */
static struct thread *cpu_info_to_thread(const struct cpu_info *ci)
{
	return ci->thread;
}

static inline struct thread *get_main_thread(void)
{
	return &all_threads[0];
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

static inline struct thread *get_free_thread(void)
{
	struct thread *t;
	struct cpu_info *ci;
	struct cpu_info *new_ci;

	if (thread_list_empty(&free_threads))
		return NULL;

	t = pop_thread(&free_threads);

	ci = cpu_info();

	/* The BSP is the only one that can switch threads. Therefore, only
	 * initialize the device_t and thread pointer. */
	new_ci = thread_cpu_info(t);
	new_ci->cpu = ci->cpu;
	new_ci->thread = t;

	return t;
}

static inline void free_thread(struct thread *t)
{
	push_thread(&free_threads, t);
}

void threads_initialize(void)
{
	int i;
	struct thread *t;
	char *stack_top;
	struct cpu_info *ci;

	/* Initialize the BSP thread first. */
	t = &all_threads[0];
	ci = cpu_info();
	ci->thread = t;
	t->stack_top = (void *)&ci[1];
	t->id = 0;

	stack_top = &thread_stacks[CONFIG_STACK_SIZE];
	for (i = 1; i < TOTAL_NUM_THREADS; i++) {
		t = &all_threads[i];
		t->stack_top = stack_top;
		t->id = i;
		stack_top += CONFIG_STACK_SIZE;
		free_thread(t);
	}
}

#if 0
static void debug_thread_dump(const char *str, const struct thread *t,
                              uintptr_t stack_start)
{
	printk(BIOS_DEBUG, "%s: thread[%d] stack @%x -> %p\n", str, t->id,
	       stack_start, t->stack_top);

	int i;
	uint32_t *slot;
	uint32_t *end;

	slot = (void *)t->stack_top;
	slot--;
	end = (void *)stack_start;
	end--;
	i = (uintptr_t)t->stack_top - stack_start;
	i -= sizeof(slot);

	while (slot != end) {
		printk(BIOS_DEBUG, "%04x: %08x\n", i, *slot);
		slot--;
		i -= sizeof(*slot);
	}
}
#endif

static void schedule(struct thread *t)
{
	struct thread *current;

	current = current_thread();

	/* If t is NULL need to find new runnable thread. */
	if (t == NULL) {
		if (thread_list_empty(&runnable_threads))
			die("Runnable thread list is empty!\n");
		t = pop_thread(&runnable_threads);
		switch_to_thread(t->saved_stack, &current->saved_stack);
	} else {
		push_thread(&runnable_threads, current);
		switch_to_thread(t->saved_stack, &current->saved_stack);
	}
}

static void terminate_thread(struct thread *t)
{
	free_thread(t);
	schedule(NULL);
}

static void asmlinkage call_wrapper(struct thread *t)
{
	t->entry(t->entry_arg);
	terminate_thread(t);
}

static inline uintptr_t push_stack(uintptr_t cur_stack, uintptr_t value)
{
	uintptr_t *addr;

	cur_stack -= sizeof(value);
	addr = (uintptr_t *)cur_stack;
	*addr = value;
	return cur_stack;
}

static uintptr_t prepare_thread(struct thread *t, void *func, void *arg)
{
	uintptr_t stack;

	stack = (uintptr_t)thread_cpu_info(t);

	/* Stash the function and argument to run. */
	t->entry = func;
	t->entry_arg = arg;

	/* All new threads can yield by default. */
	t->can_yield = 1;

	/* Imitate call_wrapper(t) with return address of 0. call_wrapper()
	 * will never return through the stack as it will call
	 * terminate_thread().*/
	stack = push_stack(stack, (uintptr_t)t);
	stack = push_stack(stack, (uintptr_t)0);
	stack = push_stack(stack, (uintptr_t)call_wrapper);
	/* Make room for the registers. Ignore intial values. */
	stack -= sizeof(struct pushad_regs);

	return stack;
}

int thread_run(void (*func)(void *), void *arg)
{
	struct thread *current;
	struct cpu_info *ci;
	struct thread *t;

	ci = cpu_info();

	if (!cpu_info_can_switch(ci)) {
		printk(BIOS_ERR, "thread_run(): non-switchable cpu: %d\n",
		       ci->index);
		return -1;
	}

	current = cpu_info_to_thread(ci);
	t = get_free_thread();

	if (t == NULL) {
		printk(BIOS_ERR, "thread_run() No more threads!\n");
		return -1;
	}

	t->saved_stack = prepare_thread(t, func, arg);
	schedule(t);

	return 0;
}

void thread_resume(struct timeout_callback *tocb)
{
	struct thread *to;
	struct thread *current;

	current = current_thread();
	to = tocb->priv;
	schedule(to);
}

int thread_yield_for(unsigned microsecs)
{
	struct thread *current;
	struct cpu_info *ci;

	ci = cpu_info();

	if (!cpu_info_can_switch(ci))
		return -1;
	current = cpu_info_to_thread(ci);

	/* By default thread_yield_for() will not yield the main thread. */
	if (thread_is_main(current) || !current->can_yield)
		return -1;

	current->tocb.priv = current;
	current->tocb.callback = thread_resume;

	if (timer_sched_callback(&current->tocb, microsecs))
		return -1;

	schedule(NULL);

	return 0;
}

void thread_cooperate(void)
{
	struct thread *current;

	current = current_thread();

	current->can_yield = 1;
}

void thread_prevent_coop(void)
{
	struct thread *current;

	current = current_thread();

	current->can_yield = 0;
}
