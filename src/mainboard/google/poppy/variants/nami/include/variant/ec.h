/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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

#ifndef __MAINBOARD_EC_H__
#define __MAINBOARD_EC_H__

#include <ec/ec.h>
#include <ec/google/chromeec/ec_commands.h>

#include <variant/gpio.h>

#define MAINBOARD_EC_SCI_EVENTS \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_LID_CLOSED)        |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_LID_OPEN)          |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_CONNECTED)      |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_DISCONNECTED)   |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_BATTERY_LOW)       |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_BATTERY_CRITICAL)  |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_BATTERY)           |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_BATTERY_STATUS)    |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_THERMAL_THRESHOLD) |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_THROTTLE_START)    |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_THROTTLE_STOP)     |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_PD_MCU)            |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_MODE_CHANGE)       |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_MKBP))

#define MAINBOARD_EC_SMI_EVENTS \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_LID_CLOSED))

/* EC can wake from S5 with lid or power button */
#define MAINBOARD_EC_S5_WAKE_EVENTS \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_LID_OPEN) |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_POWER_BUTTON))

/*
 * EC can wake from S3 with lid or power button or key press or
 * mode change event.
 */
#define MAINBOARD_EC_S3_WAKE_EVENTS \
	(MAINBOARD_EC_S5_WAKE_EVENTS |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEY_PRESSED))

#define MAINBOARD_EC_S0IX_WAKE_EVENTS	(MAINBOARD_EC_S3_WAKE_EVENTS)

/* Log EC wake events plus EC shutdown events */
#define MAINBOARD_EC_LOG_EVENTS \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_THERMAL_SHUTDOWN) |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_BATTERY_SHUTDOWN) |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_PANIC))

/*
 * ACPI related definitions for ASL code.
 */

/* Enable EC backed ALS device in ACPI */
#define EC_ENABLE_ALS_DEVICE

/* Enable EC backed PD MCU device in ACPI */
#define EC_ENABLE_PD_MCU_DEVICE

/* Enable LID switch and provide wake pin for EC */
#define EC_ENABLE_LID_SWITCH
#define EC_ENABLE_WAKE_PIN	GPE_EC_WAKE

/* Enable Tablet switch */
#define EC_ENABLE_TABLET_EVENT

#define SIO_EC_MEMMAP_ENABLE	/* EC Memory Map Resources */
#define SIO_EC_HOST_ENABLE	/* EC Host Interface Resources */
#define SIO_EC_ENABLE_PS2K	/* Enable PS/2 Keyboard */

/* Enable EC backed Keyboard Backlight in ACPI */
#define EC_ENABLE_KEYBOARD_BACKLIGHT

#endif /* __MAINBOARD_EC_H__ */
