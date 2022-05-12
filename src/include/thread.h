/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef THREAD_H_
#define THREAD_H_

#include <arch/cpu.h>
#include <bootstate.h>
#include <types.h>

struct thread_mutex {
	bool locked;
};

enum thread_state {
	THREAD_UNINITIALIZED,
	THREAD_STARTED,
	THREAD_DONE,
};

struct thread_handle {
	enum thread_state state;
	/* Only valid when state == THREAD_DONE */
	enum cb_err error;
};

/* Run func(arg) on a new thread. Return 0 on successful start of thread, < 0
 * when thread could not be started. The thread handle if populated, will
 * reflect the state and return code of the thread.
 */
int thread_run(struct thread_handle *handle, enum cb_err (*func)(void *), void *arg);

/* thread_run_until is the same as thread_run() except that it blocks state
 * transitions from occurring in the (state, seq) pair of the boot state
 * machine. */
int thread_run_until(struct thread_handle *handle, enum cb_err (*func)(void *), void *arg,
		     boot_state_t state, boot_state_sequence_t seq);

/* Waits until the thread has terminated and returns the error code */
enum cb_err thread_join(struct thread_handle *handle);

#if ENV_SUPPORTS_COOP

struct thread {
	int id;
	uintptr_t stack_current;
	uintptr_t stack_orig;
	struct thread *next;
	enum cb_err (*entry)(void *);
	void *entry_arg;
	int can_yield;
	struct thread_handle *handle;
};

/* Return 0 on successful yield, < 0 when thread did not yield. */
int thread_yield(void);

/* Return 0 on successful yield for the given amount of time, < 0 when thread
 * did not yield. */
int thread_yield_microseconds(unsigned int microsecs);

/* Allow and prevent thread cooperation on current running thread. By default
 * all threads are marked to be cooperative. That means a thread can yield
 * to another thread at a pre-determined switch point. i.e., udelay,
 * thread_yield, or thread_yield_microseconds.
 *
 * These methods should be used to guard critical sections so a dead lock does
 * not occur. The critical sections can be nested. Just make sure the methods
 * are used in pairs.
 */
void thread_coop_enable(void);
void thread_coop_disable(void);

void thread_mutex_lock(struct thread_mutex *mutex);
void thread_mutex_unlock(struct thread_mutex *mutex);

/* Architecture specific thread functions. */
asmlinkage void switch_to_thread(uintptr_t new_stack, uintptr_t *saved_stack);
/* Set up the stack frame for a new thread so that a switch_to_thread() call
 * will enter the thread_entry() function with arg as a parameter. The
 * saved_stack field in the struct thread needs to be updated accordingly. */
void arch_prepare_thread(struct thread *t,
			 asmlinkage void (*thread_entry)(void *), void *arg);
#else
static inline int thread_yield(void)
{
	return -1;
}
static inline int thread_yield_microseconds(unsigned int microsecs)
{
	return -1;
}
static inline void thread_coop_enable(void) {}
static inline void thread_coop_disable(void) {}

static inline void thread_mutex_lock(struct thread_mutex *mutex) {}

static inline void thread_mutex_unlock(struct thread_mutex *mutex) {}
#endif

#endif /* THREAD_H_ */
