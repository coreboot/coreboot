/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _WIFI_GENERIC_PRIVATE_H_
#define _WIFI_GENERIC_PRIVATE_H_

int smbios_write_wifi(struct device *dev, int *handle, unsigned long *current);

const char *wifi_generic_acpi_name(const struct device *dev);
void wifi_generic_fill_ssdt(const struct device *dev);

#endif
