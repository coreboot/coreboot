/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_ACPI_H_
#define _SOC_SNOWRIDGE_ACPI_H_

#include <acpi/acpi.h>
#include <device/device.h>

void domain_fill_ssdt(const struct device *dev);
void pcie_rp_fill_ssdt(const struct device *dev);

#endif // _SOC_SNOWRIDGE_ACPI_H_
