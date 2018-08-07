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

#ifndef __DRIVERS_R8168_CHIP_H__
#define __DRIVERS_R8168_CHIP_H__

struct drivers_net_config {
	uint16_t customized_leds;
	unsigned wake;	/* Wake pin for ACPI _PRW */
	/*
	 * There maybe many NIC cards in a system.
	 * This parameter is for driver to identify what
	 * the device number is and the valid range is [1-10].
	 */
	uint8_t device_index;
};

#endif /* __DRIVERS_R8168_CHIP_H__ */
