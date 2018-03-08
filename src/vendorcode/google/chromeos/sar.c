/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Intel Corp.
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

#include <cbfs.h>
#include <console/console.h>
#include <lib.h>
#include <types.h>
#include <string.h>
#include <sar.h>
#include <drivers/vpd/vpd.h>

#define WIFI_SAR_CBFS_FILENAME	"wifi_sar_defaults.hex"
#define CROS_VPD_WIFI_SAR_NAME	"wifi_sar"

static int load_sar_file_from_cbfs(void *buf, size_t buffer_size)
{
	return cbfs_boot_load_file(WIFI_SAR_CBFS_FILENAME, buf,
			buffer_size, CBFS_TYPE_RAW);
}

/* Retrieve the wifi SAR limits data from VPD and decode it

For VPD: key,value pair is in this format
"wifi_sar"=[<WRDD><EWRD>][WGDS]

WIFI SAR data in CBFS file is expected in same format: [<WRDD><EWRD>][WGDS]

[<WRDD><EWRD>] = NUM_SAR_LIMITS * BYTES_PER_SAR_LIMIT bytes.
[WGDS]=[WGDS_VERSION][WGDS_DATA]

For [WGDS_VERSION] 0x00,
[WGDS_DATA] = [GROUP#0][GROUP#1][GROUP#2]

[GROUP#<i>] =
	[2.4Ghz – Max Allowed][2.4Ghz – Chain A Offset]
	[2.4Ghz – Chain B Offset][5Ghz – Max Allowed]
	[5Ghz – Chain A Offset][5Ghz – Chain B Offset]

[GROUP#0] is for FCC
[GROUP#1] is for Europe/Japan
[GROUP#2] is for ROW

*/
int get_wifi_sar_limits(struct wifi_sar_limits *sar_limits)
{
	const char *wifi_sar_limit_key = CROS_VPD_WIFI_SAR_NAME;
	/* vpd_gets() reads in one less than size characters from the VPD
	 * with a terminating null byte ('\0') stored as the last character into
	 * the buffer, thus the increasing by 1 for buffer_size. */
	const size_t buffer_size = (sizeof(struct wifi_sar_limits) /
					sizeof(uint8_t)) * 2 + 1;
	char wifi_sar_limit_str[buffer_size];
	uint8_t bin_buffer[sizeof(struct wifi_sar_limits)];
	size_t sar_cbfs_len, sar_expected_len, bin_buff_adjusted_size;

	/* keep it backward compatible. Some older platform are shipping
	   without GEO SAR and so older wifi_sar VPD key */

	sar_expected_len = buffer_size;
	bin_buff_adjusted_size = sizeof(struct wifi_sar_limits);

	if (!IS_ENABLED(CONFIG_GEO_SAR_ENABLE)) {
		sar_expected_len = buffer_size -
					sizeof(struct wifi_sar_delta_table) *
					sizeof(uint8_t) * 2;
		bin_buff_adjusted_size = sizeof(struct wifi_sar_limits) -
					 sizeof(struct wifi_sar_delta_table);
	}

	/* Try to read the SAR limit entry from VPD */
	if (!vpd_gets(wifi_sar_limit_key, wifi_sar_limit_str,
							 buffer_size, VPD_ANY)) {
		printk(BIOS_ERR, "Error: Could not locate '%s' in VPD.\n",
				wifi_sar_limit_key);

		if (!IS_ENABLED(CONFIG_WIFI_SAR_CBFS))
			return -1;

		printk(BIOS_DEBUG, "Checking CBFS for default SAR values\n");

		sar_cbfs_len = load_sar_file_from_cbfs(
					(void *) wifi_sar_limit_str,
						sar_expected_len);

		if (sar_cbfs_len != sar_expected_len) {
			printk(BIOS_ERR, "%s has bad len in CBFS\n",
					WIFI_SAR_CBFS_FILENAME);
			return -1;
		}
	} else {
		/* VPD key "wifi_sar" found. strlen is checked with addition of
		 * 1 as we have created buffer size 1 char larger for the reason
		 * mentioned at start of this function itself */
		if (strlen(wifi_sar_limit_str) + 1 != sar_expected_len) {
			printk(BIOS_ERR, "WIFI SAR key has bad len in VPD\n");
			return -1;
		}
	}

	/* Decode the heximal encoded string to binary values */
	if (hexstrtobin(wifi_sar_limit_str, bin_buffer, bin_buff_adjusted_size)
			< bin_buff_adjusted_size) {
		printk(BIOS_ERR, "Error: wifi_sar contains non-hex value!\n");
		return -1;
	}

	memset(sar_limits, 0, sizeof(*sar_limits));
	memcpy(sar_limits, bin_buffer, bin_buff_adjusted_size);
	return 0;
}
