/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SOUTHBRIDGE_INTEL_I3100_I3100_H
#define SOUTHBRIDGE_INTEL_I3100_I3100_H
#include "chip.h"

#define SATA_CMD     0x04
#define SATA_PI      0x09
#define SATA_PTIM    0x40
#define SATA_STIM    0x42
#define SATA_D1TIM   0x44
#define SATA_SYNCC   0x48
#define SATA_SYNCTIM 0x4A
#define SATA_IIOC    0x54
#define SATA_MAP     0x90
#define SATA_PCS     0x91
#define SATA_ACR0    0xA8
#define SATA_ACR1    0xAC
#define SATA_ATC     0xC0
#define SATA_ATS     0xC4
#define SATA_SP      0xD0

#define SATA_MODE_IDE  0x00
#define SATA_MODE_AHCI 0x01

void i3100_enable(device_t dev);

#endif
