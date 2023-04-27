/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_GENERIC_BAYHUB_LV2_CHIP_H__
#define __DRIVERS_GENERIC_BAYHUB_LV2_CHIP_H__

#include <stdbool.h>

/* Bayhub LV2 PCIe to SD bridge */
struct drivers_generic_bayhub_lv2_config {
	bool enable_power_saving;
};

#endif /* __DRIVERS_GENERIC_BAYHUB_LV2_CHIP_H__ */
