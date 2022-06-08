/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_GOOGLE_CHROMEEC_CHIP_H
#define EC_GOOGLE_CHROMEEC_CHIP_H

#include <device/device.h>
#include <stddef.h>

#define MAX_TYPEC_PORTS		4

struct ec_google_chromeec_config {
	/* Pointer to PMC Mux connector for each Type-C port */
	DEVTREE_CONST struct device *mux_conn[MAX_TYPEC_PORTS];
	DEVTREE_CONST struct device *retimer_conn[MAX_TYPEC_PORTS];
	bool ec_multifan_support;
};

#endif /* EC_GOOGLE_CHROMEEC_CHIP_H */
