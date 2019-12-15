/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Felix Held <felix-coreboot@felixheld.de>
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

/* Nuvoton is a Winbond spin-off, so this code is for both */

#include <stdint.h>
#include <superio/hwm5_conf.h>
#include "hwm.h"

#define HWM_BANK_SELECT	0x4e

void nuvoton_hwm_select_bank(const u16 base, const u8 bank)
{
	pnp_write_hwm5_index(base, HWM_BANK_SELECT, bank);
}
