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
 */

#ifndef ONBOARD_H
#define ONBOARD_H

#ifndef __ACPI__
void lan_init(void);

void save_chromeos_gpios(void);

void set_power_led(int state);

enum {
	LED_OFF		= 0,
	LED_ON,
	LED_BLINK,
};
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

/* USB Charger Control: GPIO26 */
#define GPIO_USB_CTL_1 26

/* IT8772F defs */
#define IT8772F_BASE 0x2e
#define IT8772F_SERIAL_DEV PNP_DEV(IT8772F_BASE, IT8772F_SP1)
#define IT8772F_GPIO_DEV PNP_DEV(IT8772F_BASE, IT8772F_GPIO)
#define IT8772F_SUPERIO_DEV PNP_DEV(IT8772F_BASE, 0)

#endif
