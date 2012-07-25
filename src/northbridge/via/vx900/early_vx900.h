/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2012 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EARLY_VX900_H
#define EARLY_VX900_H

#include "raminit.h"
#include "vx900.h"

#include <arch/io.h>
#include <devices/smbus/smbus.h>
#include <stdint.h>

#define MCU PCI_DEV(0, 0,    3)
#define GFX PCI_DEV(0, 1,    0)
#define LPC PCI_DEV(0, 0x11, 0)

void enable_smbus(void);
void dump_spd_data(void);
void spd_read(u8 addr, spd_raw_data spd);

void vx900_enable_pci_config_space(void);
void vx900_disable_legacy_rom_shadow(void);

void vx900_dram_set_gfx_resources(void);

#endif /* EARLY_VX900_H */
