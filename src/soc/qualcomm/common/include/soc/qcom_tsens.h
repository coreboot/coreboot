/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_TSENS_H_
#define _SOC_QUALCOMM_TSENS_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define TM_SN_STATUS_OFF    0x00a0
#define SN_STATUS_VALID     (1 << 21)
#define SN_STATUS_TEMP_MASK 0xFFF

struct tsens_controller {
	const char *name;
	uintptr_t tm_base;
	uintptr_t srot_base;
	uint32_t sensor_count;
};

enum sensor_type {
	TYPE_TSENS
};

struct thermal_zone_map {
	const char *label;
	enum sensor_type type;
	const void *ctrl;
	int hw_id;
	int threshold;
};

/*
 * Iterates through all defined zones and checks thresholds.
 */
void qcom_tsens_monitor_all(bool *has_crossed_threshold);

/* External Data (Defined in SoC file) */
extern const struct thermal_zone_map qcom_thermal_zones[];

#endif /* _SOC_QUALCOMM_TSENS_H_ */
