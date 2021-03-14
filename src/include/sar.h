/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _SAR_H_
#define _SAR_H_

#include <stdint.h>

#define NUM_SAR_LIMITS 4
#define BYTES_PER_SAR_LIMIT 10
enum {
	SAR_FCC,
	SAR_EUROPE_JAPAN,
	SAR_REST_OF_WORLD,
	SAR_NUM_WGDS_GROUPS
};

struct wifi_sar_delta_table {
	uint8_t version;
	struct {
		uint8_t power_max_2400mhz;
		uint8_t power_chain_a_2400mhz;
		uint8_t power_chain_b_2400mhz;
		uint8_t power_max_5200mhz;
		uint8_t power_chain_a_5200mhz;
		uint8_t power_chain_b_5200mhz;
	} __packed group[SAR_NUM_WGDS_GROUPS];
} __packed;

/* Wifi SAR limit table structure */
struct wifi_sar_limits {
	/* Total 4 SAR limit sets, each has 10 bytes */
	uint8_t sar_limit[NUM_SAR_LIMITS][BYTES_PER_SAR_LIMIT];
	struct wifi_sar_delta_table wgds;
} __packed;

/*
 * Retrieve the SAR limits data from VPD and decode it.
 * sar_limits:	Pointer to wifi_sar_limits where the resulted data is stored
 *
 * Returns: 0 on success, -1 on errors (The VPD entry doesn't exist, or the
 * VPD entry contains non-heximal value.)
 */
int get_wifi_sar_limits(struct wifi_sar_limits *sar_limits);

#define WIFI_SAR_CBFS_DEFAULT_FILENAME	"wifi_sar_defaults.hex"

const char *get_wifi_sar_cbfs_filename(void);

#endif /* _SAR_H_ */
