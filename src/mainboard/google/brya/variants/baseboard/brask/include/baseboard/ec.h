/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_EC_H__
#define __BASEBOARD_EC_H__

#include <ec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <baseboard/gpio.h>

#define MAINBOARD_EC_SCI_EVENTS \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_THERMAL_THRESHOLD) |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_THROTTLE_START)    |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_THROTTLE_STOP)     |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_MKBP)              |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_PD_MCU)            |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_MODE_CHANGE)       |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_USB_MUX))
#define MAINBOARD_EC_SMI_EVENTS 0
/* EC can wake from S5 with power button */
#define MAINBOARD_EC_S5_WAKE_EVENTS \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_POWER_BUTTON))
/* EC can wake from S3 with power button */
#define MAINBOARD_EC_S3_WAKE_EVENTS (MAINBOARD_EC_S5_WAKE_EVENTS)
#define MAINBOARD_EC_S0IX_WAKE_EVENTS \
	(MAINBOARD_EC_S3_WAKE_EVENTS |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_HANG_DETECT))
/* Log EC wake events plus EC shutdown events */
#define MAINBOARD_EC_LOG_EVENTS \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_THERMAL_SHUTDOWN) |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_PANIC))
/*
 * ACPI related definitions for ASL code.
 */
/* Enable cros_ec_keyb device */
#define EC_ENABLE_MKBP_DEVICE
#define EC_ENABLE_WAKE_PIN      GPE_EC_WAKE
/* Enable EC backed PD MCU device in ACPI */
#define EC_ENABLE_PD_MCU_DEVICE
#define SIO_EC_MEMMAP_ENABLE     /* EC Memory Map Resources */
#define SIO_EC_HOST_ENABLE       /* EC Host Interface Resources */

#define EC_ENABLE_SYNC_IRQ      /* Enable tight timestamp / wake support */

#endif /* __BASEBOARD_EC_H__ */
