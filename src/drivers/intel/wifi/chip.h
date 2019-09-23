/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _INTEL_WIFI_CHIP_H_
#define _INTEL_WIFI_CHIP_H_

struct drivers_intel_wifi_config {
	unsigned int wake; /* Wake pin for ACPI _PRW */
};

#endif /* _INTEL_WIFI_CHIP_H_ */
