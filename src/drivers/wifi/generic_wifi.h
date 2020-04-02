/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _GENERIC_WIFI_H_
#define _GENERIC_WIFI_H_

/**
 * struct generic_wifi_config - Data structure to contain common wifi config
 * @wake: Wake pin for ACPI _PRW
 * @maxsleep: Maximum sleep state to wake from
 */
struct generic_wifi_config {
	unsigned int wake;
	unsigned int maxsleep;
};

/**
 * wifi_fill_ssdt() - Fill ACPI SSDT table for WiFi controller
 * @dev: Device structure corresponding to WiFi controller.
 * @config: Common wifi config required to fill ACPI SSDT table.
 *
 * This function implements common device operation to help fill ACPI SSDT
 * table for WiFi controller.
 */
void generic_wifi_fill_ssdt(struct device *dev,
			    const struct generic_wifi_config *config);

/**
 * wifi_acpi_name() - Get ACPI name for WiFi controller
 * @dev: Device structure corresponding to WiFi controller.
 *
 * This function implements common device operation to get the ACPI name for
 * WiFi controller.
 *
 * Return: string representing the ACPI name for WiFi controller.
 */
const char *generic_wifi_acpi_name(const struct device *dev);

#endif /* _GENERIC_WIFI_H_ */
