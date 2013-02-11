/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2012 The ChromiumOS Authors.  All rights reserved.
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

#ifndef STOUT_ONBOARD_H
#define STOUT_ONBOARD_H

#include <arch/smp/mpspec.h>

#define STOUT_NIC_VENDOR_ID		0x10EC
#define STOUT_NIC_DEVICE_ID		0x8168

#define XHCI_MODE     2  // Auto
#define XHCI_PORTS    3  // Port 0 & 1
#define XHCI_PREBOOT  0  // No PreOS boot support
#define XHCI_STREAMS  1  // Sure, lets have streams

#endif
