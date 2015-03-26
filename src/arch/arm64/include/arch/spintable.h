/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google, Inc.
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
 * Foundation, Inc.
 */

#ifndef __ARCH_SPINTABLE_H__
#define __ARCH_SPINTABLE_H__

struct spintable_attributes {
	void (*entry)(void *);
	void *addr;
};

#if IS_ENABLED(CONFIG_ARM64_USE_SPINTABLE)

/* Initialize spintable with provided monitor address. */
void spintable_init(void *monitor_address);

/* Return NULL on failure, otherwise the spintable info. */
const struct spintable_attributes *spintable_get_attributes(void);

#else /* IS_ENABLED(CONFIG_ARM64_USE_SPINTABLE) */

static inline void spintable_init(void *monitor_address) {}
static inline const struct spintable_attributes *spintable_get_attributes(void)
{
	return NULL;
}

#endif /* IS_ENABLED(CONFIG_ARM64_USE_SPINTABLE) */

/* Start spinning on the non-boot CPUs. */
void spintable_start(void);

#endif /* __ARCH_SPINTABLE_H__ */
