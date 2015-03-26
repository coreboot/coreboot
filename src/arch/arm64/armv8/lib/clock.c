/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc
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
 *
 * clock.c: Functions for accessing clock and timer related registers
 * Reference: ARM Architecture Reference Manual, ARMv8-A edition
 */

#include <stdint.h>

#include <arch/clock.h>

void set_cntfrq(uint32_t freq)
{
	__asm__ __volatile__("msr cntfrq_el0, %0" :: "r"(freq));
}
