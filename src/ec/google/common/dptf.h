/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_GOOGLE_COMMON_DPTF_H
#define EC_GOOGLE_COMMON_DPTF_H

#include <device/device.h>

/* Called by google_chromeec_fill_ssdt_generator */
void ec_fill_dptf_helpers(const struct device *dev, const struct device *fan_dev);

#endif /* EC_GOOGLE_COMMON_DPTF_H */
