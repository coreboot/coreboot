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

#ifndef ARCH_CPU_INTERNAL_H
#define ARCH_CPU_INTERNAL_H

/*
 * Do the necessary work to prepare for secondary CPUs coming up. The
 * SoC will call this function before bringing up the other CPUs. The
 * entry point for the seoncdary CPUs is returned.
 */
void *prepare_secondary_cpu_startup(void);

/*
 * Code path for the non-BSP CPUs. This is an internal function used.
 */
void arch_secondary_cpu_init(void);

/* Return the top of the stack for the specified cpu. */
void *cpu_get_stack(unsigned int cpu);

/* Return the top of the exception stack for the specified cpu. */
void *cpu_get_exception_stack(unsigned int cpu);

#endif /* ARCH_CPU_INTERNAL_H */
