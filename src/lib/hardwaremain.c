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
 */


/*
 * C Bootstrap code for the coreboot
 */

#include <adainit.h>
#include <arch/exception.h>
#include <bootstate.h>
#include <compiler.h>
#include <console/console.h>
#include <console/post_codes.h>
#include <cbmem.h>
#include <version.h>
#include <device/device.h>
#include <device/pci.h>
#include <delay.h>
#include <stdlib.h>
#include <reset.h>
#include <boot/tables.h>
#include <program_loading.h>
#include <lib.h>
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
#include <arch/acpi.h>
#endif
#include <timer.h>
#include <timestamp.h>
#include <thread.h>

static boot_state_t bs_pre_device(void *arg);
static boot_state_t bs_dev_init_chips(void *arg);
static boot_state_t bs_dev_enumerate(void *arg);
static boot_state_t bs_dev_resources(void *arg);
static boot_state_t bs_dev_enable(void *arg);
static boot_state_t bs_dev_init(void *arg);
static boot_state_t bs_post_device(void *arg);
static boot_state_t bs_os_resume_check(void *arg);
static boot_state_t bs_os_resume(void *arg);
static boot_state_t bs_write_tables(void *arg);
static boot_state_t bs_payload_load(void *arg);
static boot_state_t bs_payload_boot(void *arg);

/*
 * Typically a state will take 4 time samples:
 *   1. Before state entry callbacks
 *   2. After state entry callbacks / Before state function.
 *   3. After state function / Before state exit callbacks.
 *   4. After state exit callbacks.
 */
#define MAX_TIME_SAMPLES 4
struct boot_state_times {
	int num_samples;
	struct mono_time samples[MAX_TIME_SAMPLES];
};

/* The prologue (BS_ON_ENTRY) and epilogue (BS_ON_EXIT) of a state can be
 * blocked from transitioning to the next (state,seq) pair. When the blockers
 * field is 0 a transition may occur. */
struct boot_phase {
	struct boot_state_callback *callbacks;
	int blockers;
};

struct boot_state {
	const char *name;
	boot_state_t id;
	u8 post_code;
	struct boot_phase phases[2];
	boot_state_t (*run_state)(void *arg);
	void *arg;
	int complete : 1;
#if IS_ENABLED(CONFIG_HAVE_MONOTONIC_TIMER)
	struct boot_state_times times;
#endif
};

#define BS_INIT(state_, run_func_)				\
	{							\
		.name = #state_,				\
		.id = state_,					\
		.post_code = POST_ ## state_,			\
		.phases = { { NULL, 0 }, { NULL, 0 } },		\
		.run_state = run_func_,				\
		.arg = NULL,					\
		.complete = 0,					\
	}
#define BS_INIT_ENTRY(state_, run_func_)	\
	[state_] = BS_INIT(state_, run_func_)

static struct boot_state boot_states[] = {
	BS_INIT_ENTRY(BS_PRE_DEVICE, bs_pre_device),
	BS_INIT_ENTRY(BS_DEV_INIT_CHIPS, bs_dev_init_chips),
	BS_INIT_ENTRY(BS_DEV_ENUMERATE, bs_dev_enumerate),
	BS_INIT_ENTRY(BS_DEV_RESOURCES, bs_dev_resources),
	BS_INIT_ENTRY(BS_DEV_ENABLE, bs_dev_enable),
	BS_INIT_ENTRY(BS_DEV_INIT, bs_dev_init),
	BS_INIT_ENTRY(BS_POST_DEVICE, bs_post_device),
	BS_INIT_ENTRY(BS_OS_RESUME_CHECK, bs_os_resume_check),
	BS_INIT_ENTRY(BS_OS_RESUME, bs_os_resume),
	BS_INIT_ENTRY(BS_WRITE_TABLES, bs_write_tables),
	BS_INIT_ENTRY(BS_PAYLOAD_LOAD, bs_payload_load),
	BS_INIT_ENTRY(BS_PAYLOAD_BOOT, bs_payload_boot),
};

void __weak arch_bootstate_coreboot_exit(void) { }

static boot_state_t bs_pre_device(void *arg)
{
	return BS_DEV_INIT_CHIPS;
}

static boot_state_t bs_dev_init_chips(void *arg)
{
	timestamp_add_now(TS_DEVICE_ENUMERATE);

	/* Initialize chips early, they might disable unused devices. */
	dev_initialize_chips();

	return BS_DEV_ENUMERATE;
}

static boot_state_t bs_dev_enumerate(void *arg)
{
	/* Find the devices we don't have hard coded knowledge about. */
	dev_enumerate();

	return BS_DEV_RESOURCES;
}

static boot_state_t bs_dev_resources(void *arg)
{
	timestamp_add_now(TS_DEVICE_CONFIGURE);

	/* Now compute and assign the bus resources. */
	dev_configure();

	return BS_DEV_ENABLE;
}

static boot_state_t bs_dev_enable(void *arg)
{
	timestamp_add_now(TS_DEVICE_ENABLE);

	/* Now actually enable devices on the bus */
	dev_enable();

	return BS_DEV_INIT;
}

static boot_state_t bs_dev_init(void *arg)
{
	timestamp_add_now(TS_DEVICE_INITIALIZE);

	/* And of course initialize devices on the bus */
	dev_initialize();

	return BS_POST_DEVICE;
}

static boot_state_t bs_post_device(void *arg)
{
	dev_finalize();
	timestamp_add_now(TS_DEVICE_DONE);

	return BS_OS_RESUME_CHECK;
}

static boot_state_t bs_os_resume_check(void *arg)
{
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
	void *wake_vector;

	wake_vector = acpi_find_wakeup_vector();

	if (wake_vector != NULL) {
		boot_states[BS_OS_RESUME].arg = wake_vector;
		return BS_OS_RESUME;
	}

	acpi_prepare_resume_backup();
#endif
	timestamp_add_now(TS_CBMEM_POST);

	return BS_WRITE_TABLES;
}

static boot_state_t bs_os_resume(void *wake_vector)
{
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
	arch_bootstate_coreboot_exit();
	acpi_resume(wake_vector);
#endif
	return BS_WRITE_TABLES;
}

static boot_state_t bs_write_tables(void *arg)
{
	timestamp_add_now(TS_WRITE_TABLES);

	/* Now that we have collected all of our information
	 * write our configuration tables.
	 */
	write_tables();

	timestamp_add_now(TS_FINALIZE_CHIPS);
	dev_finalize_chips();

	return BS_PAYLOAD_LOAD;
}

static boot_state_t bs_payload_load(void *arg)
{
	payload_load();

	return BS_PAYLOAD_BOOT;
}

static boot_state_t bs_payload_boot(void *arg)
{
	arch_bootstate_coreboot_exit();
	payload_run();

	printk(BIOS_EMERG, "Boot failed\n");
	/* Returning from this state will fail because the following signals
	 * return to a completed state. */
	return BS_PAYLOAD_BOOT;
}

#if IS_ENABLED(CONFIG_HAVE_MONOTONIC_TIMER)
static void bs_sample_time(struct boot_state *state)
{
	struct mono_time *mt;

	mt = &state->times.samples[state->times.num_samples];
	timer_monotonic_get(mt);
	state->times.num_samples++;
}

static void bs_report_time(struct boot_state *state)
{
	long entry_time;
	long run_time;
	long exit_time;
	struct mono_time *samples = &state->times.samples[0];

	entry_time = mono_time_diff_microseconds(&samples[0], &samples[1]);
	run_time = mono_time_diff_microseconds(&samples[1], &samples[2]);
	exit_time = mono_time_diff_microseconds(&samples[2], &samples[3]);

	printk(BIOS_DEBUG, "BS: %s times (us): entry %ld run %ld exit %ld\n",
	       state->name, entry_time, run_time, exit_time);
}
#else
static inline void bs_sample_time(struct boot_state *state) {}
static inline void bs_report_time(struct boot_state *state) {}
#endif

#if IS_ENABLED(CONFIG_TIMER_QUEUE)
static void bs_run_timers(int drain)
{
	/* Drain all timer callbacks until none are left, if directed.
	 * Otherwise run the timers only once. */
	do {
		if (!timers_run())
			break;
	} while (drain);
}
#else
static void bs_run_timers(int drain) {}
#endif

static void bs_call_callbacks(struct boot_state *state,
			      boot_state_sequence_t seq)
{
	struct boot_phase *phase = &state->phases[seq];

	while (1) {
		if (phase->callbacks != NULL) {
			struct boot_state_callback *bscb;

			/* Remove the first callback. */
			bscb = phase->callbacks;
			phase->callbacks = bscb->next;
			bscb->next = NULL;

#if IS_ENABLED(CONFIG_DEBUG_BOOT_STATE)
			printk(BIOS_DEBUG, "BS: callback (%p) @ %s.\n",
				bscb, bscb->location);
#endif
			bscb->callback(bscb->arg);
			continue;
		}

		/* All callbacks are complete and there are no blockers for
		 * this state. Therefore, this part of the state is complete. */
		if (!phase->blockers)
			break;

		/* Something is blocking this state from transitioning. As
		 * there are no more callbacks a pending timer needs to be
		 * ran to unblock the state. */
		bs_run_timers(0);
	}
}

/* Keep track of the current state. */
static struct state_tracker {
	boot_state_t state_id;
	boot_state_sequence_t seq;
} current_phase = {
	.state_id = BS_PRE_DEVICE,
	.seq = BS_ON_ENTRY,
};

static void bs_walk_state_machine(void)
{

	while (1) {
		struct boot_state *state;
		boot_state_t next_id;

		state = &boot_states[current_phase.state_id];

		if (state->complete) {
			printk(BIOS_EMERG, "BS: %s state already executed.\n",
			       state->name);
			break;
		}

		if (IS_ENABLED(CONFIG_DEBUG_BOOT_STATE))
			printk(BIOS_DEBUG, "BS: Entering %s state.\n",
				state->name);

		bs_run_timers(0);

		bs_sample_time(state);

		bs_call_callbacks(state, current_phase.seq);
		/* Update the current sequence so that any calls to block the
		 * current state from the run_state() function will place a
		 * block on the correct phase. */
		current_phase.seq = BS_ON_EXIT;

		bs_sample_time(state);

		post_code(state->post_code);

		next_id = state->run_state(state->arg);

		if (IS_ENABLED(CONFIG_DEBUG_BOOT_STATE))
			printk(BIOS_DEBUG, "BS: Exiting %s state.\n",
			state->name);

		bs_sample_time(state);

		bs_call_callbacks(state, current_phase.seq);

		if (IS_ENABLED(CONFIG_DEBUG_BOOT_STATE))
			printk(BIOS_DEBUG,
				"----------------------------------------\n");

		/* Update the current phase with new state id and sequence. */
		current_phase.state_id = next_id;
		current_phase.seq = BS_ON_ENTRY;

		bs_sample_time(state);

		bs_report_time(state);

		state->complete = 1;
	}
}

static int boot_state_sched_callback(struct boot_state *state,
				     struct boot_state_callback *bscb,
				     boot_state_sequence_t seq)
{
	if (state->complete) {
		printk(BIOS_WARNING,
		       "Tried to schedule callback on completed state %s.\n",
		       state->name);

		return -1;
	}

	bscb->next = state->phases[seq].callbacks;
	state->phases[seq].callbacks = bscb;

	return 0;
}

int boot_state_sched_on_entry(struct boot_state_callback *bscb,
			      boot_state_t state_id)
{
	struct boot_state *state = &boot_states[state_id];

	return boot_state_sched_callback(state, bscb, BS_ON_ENTRY);
}

int boot_state_sched_on_exit(struct boot_state_callback *bscb,
			     boot_state_t state_id)
{
	struct boot_state *state = &boot_states[state_id];

	return boot_state_sched_callback(state, bscb, BS_ON_EXIT);
}

static void boot_state_schedule_static_entries(void)
{
	extern struct boot_state_init_entry *_bs_init_begin[];
	struct boot_state_init_entry **slot;

	for (slot = &_bs_init_begin[0]; *slot != NULL; slot++) {
		struct boot_state_init_entry *cur = *slot;

		if (cur->when == BS_ON_ENTRY)
			boot_state_sched_on_entry(&cur->bscb, cur->state);
		else
			boot_state_sched_on_exit(&cur->bscb, cur->state);
	}
}

void main(void)
{
	/*
	 * We can generally jump between C and Ada code back and forth
	 * without trouble. But since we don't have an Ada main() we
	 * have to do some Ada package initializations that GNAT would
	 * do there. This has to be done before calling any Ada code.
	 *
	 * The package initializations should not have any dependen-
	 * cies on C code. So we can call them here early, and don't
	 * have to worry at which point we can start to use Ada.
	 */
	ramstage_adainit();

	/* TODO: Understand why this is here and move to arch/platform code. */
	/* For MMIO UART this needs to be called before any other printk. */
	if (IS_ENABLED(CONFIG_ARCH_X86))
		init_timer();

	/* console_init() MUST PRECEDE ALL printk()! Additionally, ensure
	 * it is the very first thing done in ramstage.*/
	console_init();

	post_code(POST_CONSOLE_READY);

	/*
	 * CBMEM needs to be recovered in the EARLY_CBMEM_INIT case because
	 * timestamps, APCI, etc rely on the cbmem infrastructure being
	 * around. Explicitly recover it.
	 */
	if (IS_ENABLED(CONFIG_EARLY_CBMEM_INIT))
		cbmem_initialize();

	/* Record current time, try to locate timestamps in CBMEM. */
	timestamp_init(timestamp_get());

	timestamp_add_now(TS_START_RAMSTAGE);
	post_code(POST_ENTRY_RAMSTAGE);

	/* Handoff sleep type from romstage. */
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
	acpi_is_wakeup();
#endif

	exception_init();
	threads_initialize();

	/* Schedule the static boot state entries. */
	boot_state_schedule_static_entries();

	bs_walk_state_machine();

	die("Boot state machine failure.\n");
}


int boot_state_block(boot_state_t state, boot_state_sequence_t seq)
{
	struct boot_phase *bp;

	/* Blocking a previously ran state is not appropriate. */
	if (current_phase.state_id > state ||
	    (current_phase.state_id == state && current_phase.seq > seq)) {
		printk(BIOS_WARNING,
		       "BS: Completed state (%d, %d) block attempted.\n",
		       state, seq);
		return -1;
	}

	bp = &boot_states[state].phases[seq];
	bp->blockers++;

	return 0;
}

int boot_state_unblock(boot_state_t state, boot_state_sequence_t seq)
{
	struct boot_phase *bp;

	/* Blocking a previously ran state is not appropriate. */
	if (current_phase.state_id > state ||
	    (current_phase.state_id == state && current_phase.seq > seq)) {
		printk(BIOS_WARNING,
		       "BS: Completed state (%d, %d) unblock attempted.\n",
		       state, seq);
		return -1;
	}

	bp = &boot_states[state].phases[seq];

	if (bp->blockers == 0) {
		printk(BIOS_WARNING,
		       "BS: Unblock attempted on non-blocked state (%d, %d).\n",
		       state, seq);
		return -1;
	}

	bp->blockers--;

	return 0;
}

void boot_state_current_block(void)
{
	boot_state_block(current_phase.state_id, current_phase.seq);
}

void boot_state_current_unblock(void)
{
	boot_state_unblock(current_phase.state_id, current_phase.seq);
}
