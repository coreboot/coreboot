/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __MAINBOARD_EC_H__
#define __MAINBOARD_EC_H__

#include <ec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <baseboard/gpio.h>
#include <soc/gpio.h>

#define MAINBOARD_EC_SCI_EVENTS                                                                \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_CONNECTED)                                        \
	 | EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_DISCONNECTED)                                   \
	 | EC_HOST_EVENT_MASK(EC_HOST_EVENT_THERMAL_THRESHOLD)                                 \
	 | EC_HOST_EVENT_MASK(EC_HOST_EVENT_THROTTLE_START)                                    \
	 | EC_HOST_EVENT_MASK(EC_HOST_EVENT_THROTTLE_STOP)                                     \
	 | EC_HOST_EVENT_MASK(EC_HOST_EVENT_PD_MCU)                                            \
	 | EC_HOST_EVENT_MASK(EC_HOST_EVENT_MODE_CHANGE)                                       \
	 | EC_HOST_EVENT_MASK(EC_HOST_EVENT_USB_MUX))

#define MAINBOARD_EC_SMI_EVENTS (EC_HOST_EVENT_MASK(EC_HOST_EVENT_LID_CLOSED))

/* EC can wake from S5 with power button */
#define MAINBOARD_EC_S5_WAKE_EVENTS EC_HOST_EVENT_MASK(EC_HOST_EVENT_POWER_BUTTON)

/* EC can wake from S3 with lid, power button or mode change event */
#define MAINBOARD_EC_S3_WAKE_EVENTS                                                            \
	(MAINBOARD_EC_S5_WAKE_EVENTS | EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_CONNECTED)          \
	 | EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_DISCONNECTED)                                   \
	 | EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEY_PRESSED)                                       \
	 | EC_HOST_EVENT_MASK(EC_HOST_EVENT_MODE_CHANGE))

#define MAINBOARD_EC_S0IX_WAKE_EVENTS (MAINBOARD_EC_S3_WAKE_EVENTS)

/* Log EC wake events plus EC shutdown events */
#define MAINBOARD_EC_LOG_EVENTS                                                                \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_THERMAL_SHUTDOWN)                                    \
	 | EC_HOST_EVENT_MASK(EC_HOST_EVENT_PANIC))

/*
 * ACPI related definitions for ASL code.
 */

/* Set GPI for SCI */
#define EC_SCI_GPI GEVENT_24 /* eSPI system event -> GPE 24 */

#define SIO_EC_MEMMAP_ENABLE /* EC Memory Map Resources */
#define SIO_EC_HOST_ENABLE   /* EC Host Interface Resources */

/* Enable EC sync interrupt */
#define EC_ENABLE_SYNC_IRQ_GPIO

/* EC sync irq */
#define EC_SYNC_IRQ GPIO_84

/* Enable EC backed PD MCU device in ACPI */
#define EC_ENABLE_PD_MCU_DEVICE

#endif /* __MAINBOARD_EC_H__ */
