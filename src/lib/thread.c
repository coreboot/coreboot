/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <bootstate.h>
#include <console/console.h>
#include <smp/node.h>
#include <thread.h>
#include <timer.h>
#include <types.h>

static u8 thread_stacks[CONFIG_STACK_SIZE * CONFIG_NUM_THREADS] __aligned(sizeof(uint64_t));
static bool initialized;

static void idle_thread_init(void);

/* There needs to be at least one thread to run the ramstate state machine. */
#define TOTAL_NUM_THREADS (CONFIG_NUM_THREADS + 1)

/* Storage space for the thread structs .*/
static struct thread all_threads[TOTAL_NUM_THREADS];

/* All runnable (but not running) and free threads are kept on their
 * respective lists. */
static struct thread *runnable_threads;
static struct thread *free_threads;

static struct thread *active_thread;

static inline int thread_can_yield(const struct thread *t)
{
	return (t != NULL && t->can_yield > 0);
}

static inline void set_current_thread(struct thread *t)
{
	assert(boot_cpu());
	active_thread = t;
}

static inline struct thread *current_thread(void)
{
	if (!initialized || !boot_cpu())
		return NULL;

	return active_thread;
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

	if (thread_list_empty(&free_threads))
		return NULL;

	t = pop_thread(&free_threads);

	/* Reset the current stack value to the original. */
	if (!t->stack_orig)
		die("%s: Invalid stack value\n", __func__);

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
__noreturn static enum cb_err idle_thread(void *unused)
{
	/* This thread never voluntarily yields. */
	thread_coop_disable();
	while (1)
		timers_run();
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

	if (t->handle)
		t->handle->state = THREAD_STARTED;

	set_current_thread(t);

	switch_to_thread(t->stack_current, &current->stack_current);
}

static void terminate_thread(struct thread *t, enum cb_err error)
{
	if (t->handle) {
		t->handle->error = error;
		t->handle->state = THREAD_DONE;
	}

	free_thread(t);
	schedule(NULL);
}

static void asmlinkage call_wrapper(void *unused)
{
	struct thread *current = current_thread();
	enum cb_err error;

	error = current->entry(current->entry_arg);

	terminate_thread(current, error);
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
	enum cb_err error;

	boot_state_block(bbs->state, bbs->seq);
	error = current->entry(current->entry_arg);
	boot_state_unblock(bbs->state, bbs->seq);
	terminate_thread(current, error);
}

/* Prepare a thread so that it starts by executing thread_entry(thread_arg).
 * Within thread_entry() it will call func(arg). */
static void prepare_thread(struct thread *t, struct thread_handle *handle,
			   enum cb_err (*func)(void *), void *arg,
			   asmlinkage void (*thread_entry)(void *), void *thread_arg)
{
	/* Stash the function and argument to run. */
	t->entry = func;
	t->entry_arg = arg;

	/* All new threads can yield by default. */
	t->can_yield = 1;

	/* Pointer used to publish the state of thread */
	t->handle = handle;

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

	if (t == NULL)
		die("No threads available for idle thread!\n");

	/* Queue idle thread to run once all other threads have yielded. */
	prepare_thread(t, NULL, idle_thread, NULL, call_wrapper, NULL);
	push_runnable(t);
}

/* Don't inline this function so the timeout_callback won't have its storage
 * space on the stack cleaned up before the call to schedule(). */
static int __attribute__((noinline))
thread_yield_timed_callback(struct timeout_callback *tocb,
	unsigned int microsecs)
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

static void threads_initialize(void)
{
	int i;
	struct thread *t;
	u8 *stack_top;

	if (initialized)
		return;

	t = &all_threads[0];

	set_current_thread(t);

	t->stack_orig = (uintptr_t)NULL; /* We never free the main thread */
	t->id = 0;
	t->can_yield = 1;

	stack_top = &thread_stacks[CONFIG_STACK_SIZE];
	for (i = 1; i < TOTAL_NUM_THREADS; i++) {
		t = &all_threads[i];
		t->stack_orig = (uintptr_t)stack_top;
		t->id = i;
		stack_top += CONFIG_STACK_SIZE;
		free_thread(t);
	}

	idle_thread_init();

	initialized = 1;
}

int thread_run(struct thread_handle *handle, enum cb_err (*func)(void *), void *arg)
{
	struct thread *current;
	struct thread *t;

	/* Lazy initialization */
	threads_initialize();

	current = current_thread();

	if (!thread_can_yield(current)) {
		printk(BIOS_ERR, "%s() called from non-yielding context!\n", __func__);
		return -1;
	}

	t = get_free_thread();

	if (t == NULL) {
		printk(BIOS_ERR, "%s: No more threads!\n", __func__);
		return -1;
	}

	prepare_thread(t, handle, func, arg, call_wrapper, NULL);
	schedule(t);

	return 0;
}

int thread_run_until(struct thread_handle *handle, enum cb_err (*func)(void *), void *arg,
		     boot_state_t state, boot_state_sequence_t seq)
{
	struct thread *current;
	struct thread *t;
	struct block_boot_state *bbs;

	/* This is a ramstage specific API */
	if (!ENV_RAMSTAGE)
		dead_code();

	/* Lazy initialization */
	threads_initialize();

	current = current_thread();

	if (!thread_can_yield(current)) {
		printk(BIOS_ERR, "%s() called from non-yielding context!\n", __func__);
		return -1;
	}

	t = get_free_thread();

	if (t == NULL) {
		printk(BIOS_ERR, "%s: No more threads!\n", __func__);
		return -1;
	}

	bbs = thread_alloc_space(t, sizeof(*bbs));
	bbs->state = state;
	bbs->seq = seq;
	prepare_thread(t, handle, func, arg, call_wrapper_block_state, bbs);
	schedule(t);

	return 0;
}

int thread_yield(void)
{
	return thread_yield_microseconds(0);
}

int thread_yield_microseconds(unsigned int microsecs)
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

void thread_coop_enable(void)
{
	struct thread *current;

	current = current_thread();

	if (current == NULL)
		return;

	assert(current->can_yield <= 0);

	current->can_yield++;
}

void thread_coop_disable(void)
{
	struct thread *current;

	current = current_thread();

	if (current == NULL)
		return;

	current->can_yield--;
}

enum cb_err thread_join(struct thread_handle *handle)
{
	struct stopwatch sw;
	struct thread *current = current_thread();

	assert(handle);
	assert(current);
	assert(current->handle != handle);

	if (handle->state == THREAD_UNINITIALIZED)
		return CB_ERR_ARG;

	printk(BIOS_SPEW, "waiting for thread\n");

	stopwatch_init(&sw);

	while (handle->state != THREAD_DONE)
		assert(thread_yield() == 0);

	printk(BIOS_SPEW, "took %lld us\n", stopwatch_duration_usecs(&sw));

	return handle->error;
}

void thread_mutex_lock(struct thread_mutex *mutex)
{
	struct stopwatch sw;

	stopwatch_init(&sw);

	while (mutex->locked)
		assert(thread_yield() == 0);
	mutex->locked = true;

	printk(BIOS_SPEW, "took %lld us to acquire mutex\n", stopwatch_duration_usecs(&sw));
}

void thread_mutex_unlock(struct thread_mutex *mutex)
{
	assert(mutex->locked);
	mutex->locked = 0;
}
