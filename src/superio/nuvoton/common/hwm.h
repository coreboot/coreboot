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

#ifndef SUPERIO_NUVOTON_COMMON_HWM_H
#define SUPERIO_NUVOTON_COMMON_HWM_H

/* Nuvoton is a Winbond spin-off, so this code is for both */

#include <stdint.h>

void nuvoton_hwm_select_bank(const u16 base, const u8 bank);

#endif /* SUPERIO_NUVOTON_COMMON_HWM_H */
