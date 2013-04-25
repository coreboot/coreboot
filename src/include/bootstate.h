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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef BOOTSTATE_H
#define BOOTSTATE_H

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

typedef enum {
	BS_ON_ENTRY,
	BS_ON_EXIT
} boot_state_sequence_t;

struct boot_state_callback {
	void *arg;
	void (*callback)(void *arg);
	/* For use internal to the boot state machine. */
	struct boot_state_callback *next;
};

#define BOOT_STATE_CALLBACK_INIT(func_, arg_)	\
	{					\
		.arg = arg_,			\
		.callback = func_,		\
		.next = NULL,			\
	}
#define BOOT_STATE_CALLBACK(name_, func_, arg_)	\
	struct boot_state_callback name_ = BOOT_STATE_CALLBACK_INIT(func_, arg_)

/* The following 2 functions schedule a callback to be called on entry/exit
 * to a given state. Note that thare are no ordering guarantees between the
 * individual callbacks. 0 is returned on success < 0  on error. */
int boot_state_on_entry(struct boot_state_callback *bscb, boot_state_t state);
int boot_state_on_exit(struct boot_state_callback *bscb, boot_state_t state);

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

#define BOOT_STATE_INIT_ATTR  __attribute__ ((section (".bs_init")))

#define BOOT_STATE_INIT_ENTRIES(name_) \
	struct boot_state_init_entry name_[] BOOT_STATE_INIT_ATTR

#define BOOT_STATE_INIT_ENTRY(state_, when_, func_, arg_)	\
	{							\
		.state = state_,				\
		.when = when_,					\
		.bscb = BOOT_STATE_CALLBACK_INIT(func_, arg_),	\
	}

#endif /* BOOTSTATE_H */
