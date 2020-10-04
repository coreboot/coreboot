/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _WIFI_GENERIC_H_
#define _WIFI_GENERIC_H_

/**
 * struct drivers_wifi_generic_config - Data structure to contain generic wifi config
 * @wake: Wake pin for ACPI _PRW
 */
struct drivers_wifi_generic_config {
	unsigned int wake;
};

/**
 * wifi_generic_fill_ssdt() - Fill ACPI SSDT table for WiFi controller
 * @dev: Device structure corresponding to WiFi controller.
 * @config: Generic wifi config required to fill ACPI SSDT table.
 *
 * This function implements common device operation to help fill ACPI SSDT
 * table for WiFi controller.
 */
void wifi_generic_fill_ssdt(const struct device *dev,
			    const struct drivers_wifi_generic_config *config);

/**
 * wifi_generic_acpi_name() - Get ACPI name for WiFi controller
 * @dev: Device structure corresponding to WiFi controller.
 *
 * This function implements common device operation to get the ACPI name for
 * WiFi controller.
 *
 * Return: string representing the ACPI name for WiFi controller.
 */
const char *wifi_generic_acpi_name(const struct device *dev);

#endif /* _GENERIC_WIFI_H_ */
