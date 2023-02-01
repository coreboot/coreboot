/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef BOOTSTATE_H
#define BOOTSTATE_H

#include <assert.h>
#include <string.h>
#include <stddef.h>
/* Only declare main() when in ramstage. */
#if ENV_RAMSTAGE
#include <main_decl.h>
#endif

/*
 * The boot state machine provides a mechanism for calls to be made through-
 * out the main boot process. The boot process is separated into discrete
 * states. Upon a state's entry and exit and callbacks can be made. For
 * example:
 *
 *      Enter State
 *           +
 *           |
 *           V
 *   +-----------------+
 *   | Entry callbacks |
 *   +-----------------+
 *   | State Actions   |
 *   +-----------------+
 *   | Exit callbacks  |
 *   +-------+---------+
 *           |
 *           V
 *       Next State
 *
 * Below is the current flow from top to bottom:
 *
 *        start
 *          |
 *    BS_PRE_DEVICE
 *          |
 *    BS_DEV_INIT_CHIPS
 *          |
 *    BS_DEV_ENUMERATE
 *          |
 *    BS_DEV_RESOURCES
 *          |
 *    BS_DEV_ENABLE
 *          |
 *    BS_DEV_INIT
 *          |
 *    BS_POST_DEVICE
 *          |
 *    BS_OS_RESUME_CHECK -------- BS_OS_RESUME
 *          |                          |
 *    BS_WRITE_TABLES              os handoff
 *          |
 *    BS_PAYLOAD_LOAD
 *          |
 *    BS_PAYLOAD_BOOT
 *          |
 *      payload run
 *
 * Brief description of states:
 *   BS_PRE_DEVICE - before any device tree actions take place
 *   BS_DEV_INIT_CHIPS - init all chips in device tree
 *   BS_DEV_ENUMERATE - device tree probing
 *   BS_DEV_RESOURCES - device tree resource allocation and assignment
 *   BS_DEV_ENABLE - device tree enabling/disabling of devices
 *   BS_DEV_INIT - device tree device initialization
 *   BS_POST_DEVICE - all device tree actions performed
 *   BS_OS_RESUME_CHECK - check for OS resume
 *   BS_OS_RESUME - resume to OS
 *   BS_WRITE_TABLES - write coreboot tables
 *   BS_PAYLOAD_LOAD - Load payload into memory
 *   BS_PAYLOAD_BOOT - Boot to payload
 */

typedef enum {
	BS_PRE_DEVICE,
	BS_DEV_INIT_CHIPS,
	BS_DEV_ENUMERATE,
	BS_DEV_RESOURCES,
	BS_DEV_ENABLE,
	BS_DEV_INIT,
	BS_POST_DEVICE,
	BS_OS_RESUME_CHECK,
	BS_OS_RESUME,
	BS_WRITE_TABLES,
	BS_PAYLOAD_LOAD,
	BS_PAYLOAD_BOOT,
} boot_state_t;

/* The boot_state_sequence_t describes when a callback is to be made. It is
 * called either before a state is entered or when a state is exited. */
typedef enum {
	BS_ON_ENTRY,
	BS_ON_EXIT
} boot_state_sequence_t;

struct boot_state_callback {
	void *arg;
	void (*callback)(void *arg);
	/* For use internal to the boot state machine. */
	struct boot_state_callback *next;
#if CONFIG(DEBUG_BOOT_STATE)
	const char *location;
#endif
};

static inline const char *bscb_location(const struct boot_state_callback *bscb)
{
#if CONFIG(DEBUG_BOOT_STATE)
	return bscb->location;
#else
	return dead_code_t(const char *);
#endif
}

#if CONFIG(DEBUG_BOOT_STATE)
#define BOOT_STATE_CALLBACK_LOC __FILE__ ":" STRINGIFY(__LINE__)
#define BOOT_STATE_CALLBACK_INIT_DEBUG .location = BOOT_STATE_CALLBACK_LOC,
#define INIT_BOOT_STATE_CALLBACK_DEBUG(bscb_)			\
	do {							\
		bscb_->location = BOOT_STATE_CALLBACK_LOC;	\
	} while (0)
#else
#define BOOT_STATE_CALLBACK_INIT_DEBUG
#define INIT_BOOT_STATE_CALLBACK_DEBUG(bscb_)
#endif

#define BOOT_STATE_CALLBACK_INIT(func_, arg_)		\
	{						\
		.arg = arg_,				\
		.callback = func_,			\
		.next = NULL,				\
		BOOT_STATE_CALLBACK_INIT_DEBUG		\
	}

#define BOOT_STATE_CALLBACK(name_, func_, arg_)	\
	struct boot_state_callback name_ = BOOT_STATE_CALLBACK_INIT(func_, arg_)

/* Initialize an allocated boot_state_callback. */
#define INIT_BOOT_STATE_CALLBACK(bscb_, func_, arg_)	\
	do {						\
		INIT_BOOT_STATE_CALLBACK_DEBUG(bscb_)	\
		bscb_->callback = func_;		\
		bscb_->arg = arg_			\
	} while (0)

/* The following 2 functions schedule a callback to be called on entry/exit
 * to a given state. Note that there are no ordering guarantees between the
 * individual callbacks on a given state. 0 is returned on success < 0 on
 * error. */
int boot_state_sched_on_entry(struct boot_state_callback *bscb,
				boot_state_t state);
int boot_state_sched_on_exit(struct boot_state_callback *bscb,
				boot_state_t state);
/* Schedule an array of entries of size num. */
struct boot_state_init_entry;
void boot_state_sched_entries(struct boot_state_init_entry *entries,
				size_t num);

/* Block/Unblock the (state, seq) pair from transitioning. Returns 0 on
 * success < 0  when the phase of the (state,seq) has already ran. */
int boot_state_block(boot_state_t state, boot_state_sequence_t seq);
int boot_state_unblock(boot_state_t state, boot_state_sequence_t seq);

/* In order to schedule boot state callbacks at compile-time specify the
 * entries in an array using the BOOT_STATE_INIT_ENTRIES and
 * BOOT_STATE_INIT_ENTRY macros below. */
struct boot_state_init_entry {
	boot_state_t state;
	boot_state_sequence_t when;
	struct boot_state_callback bscb;
};

#if ENV_RAMSTAGE
#define BOOT_STATE_INIT_ATTR  __attribute__((used, section(".bs_init")))
#else
#define BOOT_STATE_INIT_ATTR  __attribute__((unused))
#endif

#define BOOT_STATE_INIT_ENTRY(state_, when_, func_, arg_)		\
	static struct boot_state_init_entry func_ ##_## state_ ##_## when_ = \
	{								\
		.state = state_,					\
		.when = when_,						\
		.bscb = BOOT_STATE_CALLBACK_INIT(func_, arg_),		\
	};								\
	static struct boot_state_init_entry *				\
		bsie_ ## func_ ##_## state_ ##_## when_ BOOT_STATE_INIT_ATTR = \
		&func_ ##_## state_ ##_## when_;			\
	_Static_assert(!(state_ == BS_PAYLOAD_BOOT && when_ == BS_ON_EXIT), \
		       "Invalid bootstate hook");			\
	_Static_assert(!(state_ == BS_OS_RESUME && when_ == BS_ON_EXIT), \
		       "Invalid bootstate hook");

/* Hook per arch when coreboot is exiting to payload or ACPI OS resume. It's
 * the very last thing done before the transition. */
void arch_bootstate_coreboot_exit(void);

#endif /* BOOTSTATE_H */
