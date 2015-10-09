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

#ifndef __BOOTBLOCK_COMMON_H
#define __BOOTBLOCK_COMMON_H

/* These are defined as weak no-ops that can be overridden by mainboard/SoC. */
void bootblock_mainboard_early_init(void);
void bootblock_mainboard_init(void);
void bootblock_soc_init(void);

#endif	/* __BOOTBLOCK_COMMON_H */
