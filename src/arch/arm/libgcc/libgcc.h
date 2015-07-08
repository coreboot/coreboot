/*
 * This file is part of the coreboot project.
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

#ifndef __ARCH_ARM_LIBGCC_LIBGCC_H__
#define __ARCH_ARM_LIBGCC_LIBGCC_H__

#include <stdint.h>

uint64_t __umoddi3(uint64_t num, uint64_t den);
uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t *rem);

#endif
