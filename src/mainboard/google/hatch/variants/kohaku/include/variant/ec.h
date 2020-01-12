/*
 * This file is part of the coreboot project.
 *
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

#include <baseboard/ec.h>

#define EC_ENABLE_MULTIPLE_DPTF_PROFILES

#undef MAINBOARD_EC_S3_WAKE_EVENTS

#define MAINBOARD_EC_S3_WAKE_EVENTS \
	(MAINBOARD_EC_S5_WAKE_EVENTS |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEY_PRESSED) |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_MKBP)        |\
	 EC_HOST_EVENT_MASK(EC_HOST_EVENT_MODE_CHANGE))

#endif /* VARIANT_EC_H */
