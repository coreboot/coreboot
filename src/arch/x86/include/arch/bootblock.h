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
 */

#ifndef __ARCH_BOOTBLOCK_H__
#define __ARCH_BOOTBLOCK_H__

void bootblock_early_cpu_init(void);
void bootblock_early_northbridge_init(void);
void bootblock_early_southbridge_init(void);

#endif
