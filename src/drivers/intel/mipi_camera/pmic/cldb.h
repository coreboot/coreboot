/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __INTEL_MIPI_CAMERA_CLDB_H__
#define __INTEL_MIPI_CAMERA_CLDB_H__

#include <stdint.h>
#include "../ssdb.h"

#define CLDB_SIZE 32

enum pmic_type {
	CL_TYPE_UNKNOWN,
	CL_TYPE_DISCRETE,
	CL_TYPE_TPS68470
};

struct intel_cldb {
	uint8_t version;			/* Current version */
	uint8_t pmic_type;			/* Control logic type */
	uint8_t pmic_id;			/* Control logic ID */
	struct sensor_sku_info sensor_card_sku;	/* CRD Board type */
	uint8_t input_clock;			/* InputClock */
	uint8_t platform;			/* Platform information */
	uint8_t platform_sub;			/* Platform sub-categories */
	uint8_t reserved;
	uint8_t wled1_flash_maxc;		/* WLED1 Flash Max Current */
	uint8_t wled1_torch_maxc;		/* WLED1 Torch Max Current */
	uint8_t wled2_flash_maxc;		/* WLED2 Flash Max Current */
	uint8_t wled2_torch_maxc;		/* WLED2 Torch Max Current */
	uint8_t wled1_type;			/* WLED1 Type */
	uint8_t wled2_type;			/* WLED2 Type */
	uint8_t pch_clock_src;			/* PCH Clock Source */
	uint8_t reserved2[17];
} __packed;


_Static_assert(sizeof(struct intel_cldb) == CLDB_SIZE,
		"CLDB structure size must match CLDB_SIZE");

#endif
