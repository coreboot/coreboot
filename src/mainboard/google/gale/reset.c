/*
 *
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
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

#include <console/console.h>
#include <soc/iomap.h>
#include <reset.h>

void do_hard_reset(void)
{
	/*
	 * At boot time the boot loaders would have set a magic cookie
	 * here to detect watchdog reset. However, since this is a
	 * normal reset clear the magic numbers.
	 */
	write32(TCSR_BOOT_MISC_DETECT, 0);
	write32(TCSR_RESET_DEBUG_SW_ENTRY, 0);
	write32(GCNT_PSHOLD, 0);
}
