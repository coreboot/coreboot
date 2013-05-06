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
#ifndef BOOTSTATE_H
#define BOOTSTATE_H

#include <string.h>

/* Control debugging of the boot state machine. */
#define BOOT_STATE_DEBUG 0

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
#if BOOT_STATE_DEBUG
	const char *location;
#endif
};

#if BOOT_STATE_DEBUG
#define BOOT_STATE_CALLBACK_LOC __FILE__ ":" STRINGIFY(__LINE__)
#define BOOT_STATE_CALLBACK_INIT_DEBUG .location = BOOT_STATE_CALLBACK_LOC,
#define INIT_BOOT_STATE_CALLBACK_DEBUG(bscb_) \
	bscb_->location = BOOT_STATE_CALLBACK_LOC;
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
	INIT_BOOT_STATE_CALLBACK_DEBUG(bscb_)		\
	bscb_->callback = func_;			\
	bscb_->arg = arg_

/* The following 2 functions schedule a callback to be called on entry/exit
 * to a given state. Note that thare are no ordering guarantees between the
 * individual callbacks on a given state. 0 is returned on success < 0 on
 * error. */
int boot_state_sched_on_entry(struct boot_state_callback *bscb,
                              boot_state_t state);
int boot_state_sched_on_exit(struct boot_state_callback *bscb,
                             boot_state_t state);

/* Block/Unblock the (state, seq) pair from transitioning. Returns 0 on
 * success < 0  when the phase of the (state,seq) has already ran. */
int boot_state_block(boot_state_t state, boot_state_sequence_t seq);
int boot_state_unblock(boot_state_t state, boot_state_sequence_t seq);
/* Block/Unblock current state phase from transitioning. */
void boot_state_current_block(void);
void boot_state_current_unblock(void);

/* Entry into the boot state machine. */
void hardwaremain(int boot_complete);

/* In order to schedule boot state callbacks at compile-time specify the
 * entries in an array using the BOOT_STATE_INIT_ENTRIES and
 * BOOT_STATE_INIT_ENTRY macros below. */
struct boot_state_init_entry {
	boot_state_t state;
	boot_state_sequence_t when;
	struct boot_state_callback bscb;
};

#define BOOT_STATE_INIT_ATTR  __attribute__ ((used,section (".bs_init")))

#define BOOT_STATE_INIT_ENTRIES(name_) \
	static struct boot_state_init_entry name_[] BOOT_STATE_INIT_ATTR

#define BOOT_STATE_INIT_ENTRY(state_, when_, func_, arg_)	\
	{							\
		.state = state_,				\
		.when = when_,					\
		.bscb = BOOT_STATE_CALLBACK_INIT(func_, arg_),	\
	}

#endif /* BOOTSTATE_H */
