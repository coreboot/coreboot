/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _WIFI_GENERIC_PRIVATE_H_
#define _WIFI_GENERIC_PRIVATE_H_

int smbios_write_wifi_pcie(struct device *dev, int *handle, unsigned long *current);
int smbios_write_wifi_cnvi(struct device *dev, int *handle, unsigned long *current);

const char *wifi_pcie_acpi_name(const struct device *dev);
void wifi_pcie_fill_ssdt(const struct device *dev);

void wifi_cnvi_fill_ssdt(const struct device *dev);

#endif
