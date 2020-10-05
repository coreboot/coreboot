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

#endif /* _GENERIC_WIFI_H_ */
