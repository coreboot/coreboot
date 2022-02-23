/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _WIFI_GENERIC_H_
#define _WIFI_GENERIC_H_

/**
 * struct drivers_wifi_generic_config - Data structure to contain generic wifi config
 * @wake: Wake pin for ACPI _PRW
 */
struct drivers_wifi_generic_config {
	unsigned int wake;
	/* When set to true, this will add a _DSD which contains a single
	   property, `DmaProperty`, set to 1, under the ACPI Device. */
	bool is_untrusted;
};

#endif /* _GENERIC_WIFI_H_ */
