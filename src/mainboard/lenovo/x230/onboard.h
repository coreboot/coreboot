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

#ifndef BUTTERFLY_ONBOARD_H
#define BUTTERFLY_ONBOARD_H

#include <arch/smp/mpspec.h>
/* uses 7-bit I2C address */
/* must be set to edge triggered */
#define BUTTERFLY_TRACKPAD_NAME         "trackpad"
#define BUTTERFLY_TRACKPAD_I2C_ADDR     0x67
#define BUTTERFLY_TRACKPAD_IRQ          22		/* PIRQG - 22 Edge triggered */

/* defines for programming the MAC address */
#define BUTTERFLY_NIC_VENDOR_ID		0x10EC
#define BUTTERFLY_NIC_DEVICE_ID		0x8136

/* 0x00: White LINK LED and Amber ACTIVE LED */
#define BUTTERFLY_NIC_LED_MODE		0x00
#endif
