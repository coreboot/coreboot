/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
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

#ifndef PARROT_EC_H
#define PARROT_EC_H

#define EC_SCI_GPI      7   /* GPIO7 is EC_SCI# */
#define EC_SMI_GPI      8   /* GPIO8 is EC_SMI# */
#define EC_LID_GPI     15   /* GPIO15 is EC_LID_OUT# */

/* EC SMI sources TODO - make defines
 * No event  80h
*/
#define EC_NO_EVENT 0x80
/*
 * DTS temperature update  A0h
 * Decrease brightness event  A1h
 * Increase brightness event  A2h
 * Lid open  A5h
 * Lid closed  A6h
 */
#define EC_LID_CLOSE 0xA6
 /* Bluetooth wake up event  A9h
 * Display change (LCD , CRT)  ACh
 * Cpu fast event  ADh
 * Cpu slow event  ADh
 * Battery life in critical low state (LLB) B2h
 */
#define EC_BATTERY_CRITICAL 0xB2
/*
 * Battery life in low power state (LB) B3h
 * Battery Plug-In  B5h
 * Docked in request  BAh
 * Undock request  BBh
 * Power button pressed  C2h
 * AC power plug-in  C7h
 * AC power plug-out  C8h
 * Modem Ring In  CAh
 * PME signal active  CEh
 * Acer Hotkey Function – Make event D5h
 * Acer Hotkey Function – Break event D6h
 */

#ifndef __ACPI__
extern void parrot_ec_init(void);
u8 parrot_rev(void);
int parrot_ec_running_ro(void);
#endif

#endif // PARROT_EC_H
