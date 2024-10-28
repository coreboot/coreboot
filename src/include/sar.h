/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _SAR_H_
#define _SAR_H_

#include <fw_config.h>
#include <stdint.h>

#define MAX_ANT_GAINS_REVISION	2
#define MAX_DENYLIST_ENTRY	16
#define MAX_DSAR_SET_COUNT	3
#define MAX_GEO_OFFSET_REVISION	3
#define MAX_PROFILE_COUNT	9
#define MAX_SAR_REVISION	2
#define BSAR_REVISION		1
#define WBEM_REVISION		0
#define BPAG_REVISION		2
#define BBFB_REVISION		1
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
	uint8_t wgds_table[];
} __packed;

struct sar_profile {
	uint8_t revision;
	uint8_t dsar_set_count;
	uint8_t chains_count;
	uint8_t subbands_count;
	uint8_t sar_table[];
} __packed;

struct gain_profile {
	uint8_t revision;
	uint8_t mode;
	uint8_t chains_count;
	uint8_t bands_count;
	uint8_t ppag_table[];
} __packed;

struct avg_profile {
	uint8_t revision;
	uint8_t tas_selection;
	uint8_t tas_list_size;
	uint16_t deny_list_entry[MAX_DENYLIST_ENTRY];
} __packed;

struct dsm_profile {
	uint32_t supported_functions;
	uint32_t disable_active_sdr_channels;
	uint32_t support_indonesia_5g_band;
	uint32_t support_ultra_high_band;
	uint32_t regulatory_configurations;
	uint32_t uart_configurations;
	uint32_t enablement_11ax;
	uint32_t unii_4;
	uint32_t enablement_11be;
	uint32_t energy_detection_threshold;
	uint32_t rfi_mitigation;
};

struct bsar_profile {
	uint8_t revision;
	uint8_t increased_power_mode_limitation;
	uint8_t sar_lb_power_restriction;
	uint8_t br_modulation;
	uint8_t edr2_modulation;
	uint8_t edr3_modulation;
	uint8_t le_modulation;
	uint8_t le2_mhz_modulation;
	uint8_t le_lr_modulation;
} __packed;

struct wbem_profile {
	uint8_t revision;
	uint32_t bandwidth_320mhz_country_enablement;
} __packed;

struct bpag_profile {
	uint8_t revision;
	uint32_t antenna_gain_country_enablement;
} __packed;

struct bbfb_profile {
	uint8_t revision;
	uint8_t enable_quad_filter_bypass;
} __packed;

struct sar_header {
	char marker[SAR_STR_PREFIX_SIZE];
	uint8_t version;
	uint16_t offsets[];
} __packed;

/* Wifi SAR limit table structure */
union wifi_sar_limits {
	struct {
		struct sar_profile *sar;
		struct geo_profile *wgds;
		struct gain_profile *ppag;
		struct avg_profile *wtas;
		struct dsm_profile *dsm;
		struct bsar_profile *bsar;
		struct wbem_profile *wbem;
		struct bpag_profile *bpag;
		struct bbfb_profile *bbfb;
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

char *get_wifi_sar_fw_config_filename(const struct fw_config_field *field);

#endif /* _SAR_H_ */
