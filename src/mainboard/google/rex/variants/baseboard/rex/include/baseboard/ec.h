/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_EC_H__
#define __BASEBOARD_EC_H__

#include <ec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <baseboard/gpio.h>

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
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_MKBP)              |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_PD_MCU)            |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_MODE_CHANGE)       |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_USB_MUX))
#define MAINBOARD_EC_SMI_EVENTS \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_LID_CLOSED))
/* EC can wake from S5 with lid or power button */
#define MAINBOARD_EC_S5_WAKE_EVENTS \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_LID_OPEN)     |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_POWER_BUTTON))
/*
 * EC can wake from S3/S0ix with:
 * 1. Lid open
 * 2. AC Connect/Disconnect
 * 3. Power button
 * 4. Key press
 * 5. Mode change
 */
#define MAINBOARD_EC_S3_WAKE_EVENTS \
	(MAINBOARD_EC_S5_WAKE_EVENTS |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_CONNECTED)    |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_DISCONNECTED) |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEY_PRESSED)     |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_MODE_CHANGE))
#define MAINBOARD_EC_S0IX_WAKE_EVENTS \
	(MAINBOARD_EC_S3_WAKE_EVENTS |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_USB_MUX)         |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_HANG_DETECT))
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
/* Enable Keyboard Backlight */
#define EC_ENABLE_KEYBOARD_BACKLIGHT
/* Enable MKBP for buttons and switches */
#define EC_ENABLE_MKBP_DEVICE
/* Enable LID switch and provide wake pin for EC */
#define EC_ENABLE_LID_SWITCH
#define EC_ENABLE_WAKE_PIN      GPE_EC_WAKE
/* Enable EC backed PD MCU device in ACPI */
#define EC_ENABLE_PD_MCU_DEVICE
#define SIO_EC_MEMMAP_ENABLE     /* EC Memory Map Resources */
#define SIO_EC_HOST_ENABLE       /* EC Host Interface Resources */
#define SIO_EC_ENABLE_PS2K       /* Enable PS/2 Keyboard */

#define EC_ENABLE_SYNC_IRQ      /* Enable tight timestamp / wake support */

#endif /* __BASEBOARD_EC_H__ */
