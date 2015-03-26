/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef __SECMON_SECMON_H__
#define __SECMON_SECMON_H__

/*
 * The secmon_trampoline() switches mode to EL3t, reinitializing both
 * EL3t and EL3h stacks.
 */
void secmon_trampoline(void *entry, void *arg);

/* Wait for action to take place. */
void secmon_wait_for_action(void);

#endif /* __SECMON_SECMON_H__ */
