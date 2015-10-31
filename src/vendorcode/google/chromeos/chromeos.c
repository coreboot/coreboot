/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

#include <stddef.h>
#include <string.h>
#include "chromeos.h"

int __attribute__((weak)) clear_recovery_mode_switch(void)
{
	// Can be implemented by a mainboard
	return 0;
}

#ifdef __ROMSTAGE__
void __attribute__((weak)) save_chromeos_gpios(void)
{
	// Can be implemented by a mainboard
}

int __attribute__((weak)) get_sw_write_protect_state(void)
{
	// Can be implemented by a platform / mainboard
	return 0;
}
#endif
