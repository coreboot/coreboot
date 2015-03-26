/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 The ChromiumOS Authors
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

#ifndef __ARCH_STAGES_H
#define __ARCH_STAGES_H

#include <stdint.h>

extern void main(void);

void stage_entry(void);
void stage_exit(void *);
void jmp_to_elf_entry(void *entry, unsigned long buffer, unsigned long size);

/* This function is called upon initial entry of each stage. It is called prior
 * to main(). That means all of the common infrastructure will most likely not
 * be available to be used (such as console). */
void arm64_soc_init(void);

#endif
