/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corp.
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
#ifndef _SAR_H_
#define _SAR_H_

#include <stdint.h>

#define NUM_SAR_LIMITS 4
#define BYTES_PER_SAR_LIMIT 10

/* Wifi SAR limit table structure */
struct wifi_sar_limits {
	/* Total 4 SAR limit sets, each has 10 bytes */
	uint8_t sar_limit[NUM_SAR_LIMITS][BYTES_PER_SAR_LIMIT];
};

/*
 * Retrieve the SAR limits data from VPD and decode it.
 * sar_limits:	Pointer to wifi_sar_limits where the resulted data is stored
 *
 * Returns:	0 on success, -1 on errors (The VPD entry doesn't exist, or the
 * VPD entry contains non-heximal value.)
 */
int get_wifi_sar_limits(struct wifi_sar_limits *sar_limits);

#endif /* _SAR_H_ */
