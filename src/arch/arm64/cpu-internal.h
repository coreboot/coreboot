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
 */

#ifndef ARCH_CPU_INTERNAL_H
#define ARCH_CPU_INTERNAL_H

/* Return the top of the stack for the cpu. */
void *cpu_get_stack(void);

/* Return the top of the exception stack for the cpu. */
void *cpu_get_exception_stack(void);

#endif /* ARCH_CPU_INTERNAL_H */
