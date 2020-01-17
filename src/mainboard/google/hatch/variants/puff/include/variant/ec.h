/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corporation.
 * Copyright 2019 Google LLC
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

#ifndef VARIANT_EC_H
#define VARIANT_EC_H

#include <ec/google/chromeec/ec_commands.h>
#include <variant/gpio.h>

#define MAINBOARD_EC_SCI_EVENTS \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_CONNECTED)      |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_DISCONNECTED)   |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_THERMAL_THRESHOLD) |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_THROTTLE_START)    |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_THROTTLE_STOP)     |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_PD_MCU)            |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_MKBP))

#define MAINBOARD_EC_SMI_EVENTS 0

/* EC can wake from S5 with power button */
#define MAINBOARD_EC_S5_WAKE_EVENTS \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_POWER_BUTTON))

/* EC can wake from S3 with power button */
#define MAINBOARD_EC_S3_WAKE_EVENTS (MAINBOARD_EC_S5_WAKE_EVENTS)

#define MAINBOARD_EC_S0IX_WAKE_EVENTS \
	(MAINBOARD_EC_S3_WAKE_EVENTS | \
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_HANG_DETECT))

/* Log EC wake events plus EC shutdown events */
#define MAINBOARD_EC_LOG_EVENTS \
	(EC_HOST_EVENT_MASK(EC_HOST_EVENT_THERMAL_SHUTDOWN) |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_PANIC))

/*
 * ACPI related definitions for ASL code.
 */

/* Enable EC backed PD MCU device in ACPI */
#define EC_ENABLE_PD_MCU_DEVICE

/* Provide wake pin for EC for _PRW WoL method */
#define EC_ENABLE_WAKE_PIN	GPE_EC_WAKE

#define SIO_EC_MEMMAP_ENABLE	/* EC Memory Map Resources */
#define SIO_EC_HOST_ENABLE	/* EC Host Interface Resources */

/* Enable EC sync interrupt, EC_SYNC_IRQ is defined in baseboard/gpio.h */
#define EC_ENABLE_SYNC_IRQ

#endif /* VARIANT_EC_H */
