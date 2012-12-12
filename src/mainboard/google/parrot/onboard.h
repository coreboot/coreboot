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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef PARROT_ONBOARD_H
#define PARROT_ONBOARD_H

#include <arch/smp/mpspec.h>

#define PARROT_TRACKPAD_NAME         "trackpad"
#define PARROT_TRACKPAD_I2C_ADDR     0x67
#define PARROT_TRACKPAD_IRQ_DVT      16
#define PARROT_TRACKPAD_IRQ_PVT      20
#endif
