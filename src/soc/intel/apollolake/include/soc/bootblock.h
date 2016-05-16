/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_APOLLOLAKE_BOOTBLOCK_H_
#define _SOC_APOLLOLAKE_BOOTBLOCK_H_
#include <arch/cpu.h>

void asmlinkage bootblock_c_entry(uint32_t tsc_hi, uint32_t tsc_lo);
#endif
