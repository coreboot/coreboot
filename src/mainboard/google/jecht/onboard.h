/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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
 * Foundation, Inc.
 */

#ifndef ONBOARD_H
#define ONBOARD_H

#ifndef __ACPI__
void lan_init(void);
#endif

/* defines for programming the MAC address */
#define JECHT_NIC_VENDOR_ID		0x10EC
#define JECHT_NIC_DEVICE_ID		0x8168

/* 0x00: White LINK LED and Amber ACTIVE LED */
#define JECHT_NIC_LED_MODE		0x00

/* NIC wake is GPIO 8 */
#define JECHT_NIC_WAKE_GPIO		8

/* WLAN wake is GPIO 10 */
#define JECHT_WLAN_WAKE_GPIO		10

#endif
