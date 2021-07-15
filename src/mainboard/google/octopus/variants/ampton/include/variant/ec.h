/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_EC_H
#define MAINBOARD_EC_H

#include <baseboard/ec.h>

/* Enable EC sync interrupt, EC_SYNC_IRQ is defined in variant/gpio.h */
#define EC_ENABLE_SYNC_IRQ

#undef MAINBOARD_EC_S0IX_WAKE_EVENTS
#define MAINBOARD_EC_S0IX_WAKE_EVENTS \
	(MAINBOARD_EC_S3_WAKE_EVENTS |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_BATTERY_SHUTDOWN))

#endif
