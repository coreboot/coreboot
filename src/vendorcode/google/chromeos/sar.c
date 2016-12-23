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
#include <console/console.h>
#include <types.h>
#include <string.h>
#include <sar.h>
#include "cros_vpd.h"

/* Retrieve the wifi SAR limits data from VPD and decode it */
int get_wifi_sar_limits(struct wifi_sar_limits *sar_limits)
{
	const char *wifi_sar_limit_key = CROS_VPD_WIFI_SAR_NAME;
	/*
	 * cros_vpd_gets() reads in one less than size characters from the VPD
	 * with a terminating null byte ('\0') stored as the last character into
	 * the buffer, thus the increasing by 1 for buffer_size.
	 */
	const size_t buffer_size = (sizeof(struct wifi_sar_limits) /
				    sizeof(uint8_t)) * 2 + 1;
	char wifi_sar_limit_str[buffer_size];
	uint8_t bin_buffer[sizeof(struct wifi_sar_limits)];

	/* Try to read the SAR limit entry from VPD */
	if (!cros_vpd_gets(wifi_sar_limit_key, wifi_sar_limit_str,
		ARRAY_SIZE(wifi_sar_limit_str))) {
		printk(BIOS_ERR,
			"Error: Could not locate '%s' in VPD\n",
			wifi_sar_limit_key);
		return -1;
	}
	printk(BIOS_DEBUG, "VPD wifi_sar = %s\n", wifi_sar_limit_str);

	/* Decode the heximal encoded string to binary values */
	if (hexstrtobin(wifi_sar_limit_str, bin_buffer,
			sizeof(struct wifi_sar_limits))
			< sizeof(struct wifi_sar_limits)) {
		printk(BIOS_ERR,
			"Error: VPD wifi_sar contains non-heximal value!\n");
		return -1;
	}

	/* Fill the sar_limits structure with the decoded data */
	for (int i = 0; i < NUM_SAR_LIMITS; i++)
		memcpy(sar_limits->sar_limit[i],
			&bin_buffer[BYTES_PER_SAR_LIMIT * i],
			BYTES_PER_SAR_LIMIT);
	return 0;
}
