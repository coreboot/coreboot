/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _SAR_H_
#define _SAR_H_

#include <stdint.h>

#define MAX_DSAR_SET_COUNT	3
#define MAX_GEO_OFFSET_REVISION	3
#define MAX_PROFILE_COUNT	2
#define MAX_SAR_REVISION	2
#define REVISION_SIZE		1
#define SAR_REV0_CHAINS_COUNT	2
#define SAR_REV0_SUBBANDS_COUNT	5
#define SAR_FILE_REVISION	1
#define SAR_STR_PREFIX		"$SAR"
#define SAR_STR_PREFIX_SIZE	4

struct geo_profile {
	uint8_t revision;
	uint8_t chains_count;
	uint8_t bands_count;
	uint8_t wgds_table[0];
} __packed;

struct sar_profile {
	uint8_t revision;
	uint8_t dsar_set_count;
	uint8_t chains_count;
	uint8_t subbands_count;
	uint8_t sar_table[0];
} __packed;

struct sar_header {
	char marker[SAR_STR_PREFIX_SIZE];
	uint8_t version;
	uint16_t offsets[0];
} __packed;

/* Wifi SAR limit table structure */
union wifi_sar_limits {
	struct {
		struct sar_profile *sar;
		struct geo_profile *wgds;
	};
	void *profile[MAX_PROFILE_COUNT];
};

/*
 * Retrieve the wifi ACPI configuration data from CBFS and decode it
 * sar_limits:	Pointer to wifi_sar_limits where the resulted data is stored
 *
 * Returns: 0 on success, -1 on errors (The .hex file doesn't exist, or the decode failed)
 */
int get_wifi_sar_limits(union wifi_sar_limits *sar_limits);

#define WIFI_SAR_CBFS_DEFAULT_FILENAME	"wifi_sar_defaults.hex"

const char *get_wifi_sar_cbfs_filename(void);

#endif /* _SAR_H_ */
