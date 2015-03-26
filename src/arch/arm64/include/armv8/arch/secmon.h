/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#ifndef __ARCH_ARM64_ARMV8_SECMON__
#define __ARCH_ARM64_ARMV8_SECMON__

#include <arch/cpu.h>

struct secmon_params {
	size_t online_cpus;
	struct cpu_action bsp;
	struct cpu_action secondary;
};

void secmon_run(void (*entry)(void *), void *arg);
void soc_get_secmon_base_size(uint64_t *secmon_base, size_t *secmon_size);

#endif /*__ARCH_ARM64_ARMV8_SECMON__ */
