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

#ifndef LUMPY_EC_H
#define LUMPY_EC_H

#define EC_MAILBOX_PORT               0xa00 /* Mailbox IO address */
#define EC_SMI_GPI                    1     /* GPIO 1 is KBC3_EXTSMI# */

/* Commands */
#define EC_SMI_ENABLE                 0x74
#define EC_SMI_DISABLE                0x75
#define EC_ACPI_ENABLE                0x76 /* Enter ACPI mode */
#define EC_ACPI_DISABLE               0x77 /* Exit ACPI mode */

/* Commands with data */
#define EC_AUX_PORT_MODE              0x64 /* PS/2 control mode */
#define  EC_AUX_PORT_MODE_ENABLE       0x00
#define  EC_AUX_PORT_MODE_DISABLE      0x01
#define EC_POWER_BUTTON_MODE          0x63
#define  EC_POWER_BUTTON_MODE_OS       0x00 /* OS control, 8 second override */
#define  EC_POWER_BUTTON_MODE_EC       0x01 /* EC control */
#define EC_BACKLIGHT_OFF              0x67  /* Turn Backlight Off */
#define EC_BACKLIGHT_ON               0x68  /* Turn Backlight On */
#define EC_BATTERY_MODE               0x13
#define  EC_BATTERY_MODE_NORMAL        0x00 /* Normal mode */
#define  EC_BATTERY_MODE_EXTEND        0x01 /* Battery Life Cycle Extension */
#define EC_GET_SMI_CAUSE              0x72  /* Get cause of SMI */

/* EC RAM */
#define EC_FAN_SPEED                  0xca
#define  EC_FAN_SPEED_LEVEL_0	       0x01 /* Level 0 is fastest */
#define  EC_FAN_SPEED_LEVEL_1	       0x02 /* Level 1 is fast */
#define  EC_FAN_SPEED_LEVEL_2	       0x04 /* Level 2 is slow */
#define  EC_FAN_SPEED_LEVEL_3	       0x08 /* Level 3 is slowest */
#define  EC_FAN_SPEED_LEVEL_4	       0x10 /* Level 4 is off */
#define  EC_FAN_SPEED_FLAG_OS          0x80 /* OS control of fan speed */

/* EC SMI */
#define EC_LID_CLOSE                  0x9c  /* Lid close event */
#define EC_LID_OPEN                   0x9d  /* Lid open event */

extern void lumpy_ec_init(void);

#endif // LUMPY_EC_H
