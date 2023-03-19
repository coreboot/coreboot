/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_LENOVO_PMH7_CHIP_H
#define EC_LENOVO_PMH7_CHIP_H

#include <stdbool.h>

struct ec_lenovo_pmh7_config {
	bool backlight_enable;
	bool dock_event_enable;
};

#endif /* EC_LENOVO_PMH7_CHIP_H */
