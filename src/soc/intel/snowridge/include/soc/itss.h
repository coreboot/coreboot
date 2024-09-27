/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_ITSS_H_
#define _SOC_SNOWRIDGE_ITSS_H_

#include <device/device.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>

enum pirq itss_soc_get_dev_pirq(struct device *dev);

#endif // _SOC_SNOWRIDGE_ITSS_H_
