/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <lib.h>
#include <sar.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>

/*
 * Retrieve WiFi SAR limits data from CBFS and decode it
 * WiFi SAR data is expected in the format: [<WRDD><EWRD>][WGDS]
 *
 * [<WRDD><EWRD>] = NUM_SAR_LIMITS * BYTES_PER_SAR_LIMIT bytes.
 * [WGDS]=[WGDS_VERSION][WGDS_DATA]
 *
 * For [WGDS_VERSION] 0x00,
 * [WGDS_DATA] = [GROUP#0][GROUP#1][GROUP#2]
 *
 * [GROUP#<i>] =
 *      [2.4Ghz – Max Allowed][2.4Ghz – Chain A Offset]
 *      [2.4Ghz – Chain B Offset][5Ghz – Max Allowed]
 *      [5Ghz – Chain A Offset][5Ghz – Chain B Offset]
 *
 * [GROUP#0] is for FCC
 * [GROUP#1] is for Europe/Japan
 * [GROUP#2] is for ROW
*/
int get_wifi_sar_limits(struct wifi_sar_limits *sar_limits)
{
	const char *filename;
	size_t sar_str_len, sar_bin_len;
	char *sar_str;
	int ret = -1;

	filename = get_wifi_sar_cbfs_filename();
	if (filename == NULL) {
		printk(BIOS_DEBUG, "Filename missing for CBFS SAR file!\n");
		return ret;
	}

	/*
	 * If GEO_SAR_ENABLE is not selected, SAR file does not contain
	 * delta table settings.
	 */
	if (CONFIG(GEO_SAR_ENABLE))
		sar_bin_len = sizeof(struct wifi_sar_limits);
	else
		sar_bin_len = sizeof(struct wifi_sar_limits) -
				sizeof(struct wifi_sar_delta_table);

	/*
	 * Each hex digit is represented as a character in CBFS SAR file. Thus,
	 * the SAR file is double the size of its binary buffer equivalent.
	 * Hence, the buffer size allocated for SAR file is:
	 * `2 * sar_bin_len + 1`
	 * 1 additional byte is allocated to store the terminating '\0'.
	 */
	sar_str_len = 2 * sar_bin_len + 1;
	sar_str = malloc(sar_str_len);

	if (!sar_str) {
		printk(BIOS_ERR, "Failed to allocate space for SAR string!\n");
		return ret;
	}

	printk(BIOS_DEBUG, "Checking CBFS for default SAR values\n");

	if (cbfs_load(filename, sar_str, sar_str_len) != sar_str_len) {
		printk(BIOS_ERR, "%s has bad len in CBFS\n", filename);
		goto done;
	}

	memset(sar_limits, 0, sizeof(*sar_limits));
	if (hexstrtobin(sar_str, (uint8_t *)sar_limits, sar_bin_len) != sar_bin_len) {
		printk(BIOS_ERR, "Error: wifi_sar contains non-hex value!\n");
		goto done;
	}

	ret = 0;
done:
	free(sar_str);
	return ret;
}

__weak
const char *get_wifi_sar_cbfs_filename(void)
{
	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}
