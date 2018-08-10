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
 */

#ifndef __ARCH_STAGES_H
#define __ARCH_STAGES_H

#include <stdint.h>
#include <main_decl.h>

void stage_entry(void);

/* This function is the romstage platform entry point, and should contain all
   chipset and mainboard setup until DRAM is initialized and accessible. */
void platform_romstage_main(void);
/* This is an optional hook to run further chipset or mainboard code after DRAM
   and associated support frameworks (like CBMEM) have been initialized. */
void platform_romstage_postram(void);

#endif
