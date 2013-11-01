/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* The devicetree parser expects chip.h to reside directly in the path
 * specified by the devicetree. */

#ifndef _BAYTRAIL_CHIP_H_
#define _BAYTRAIL_CHIP_H_

#include <stdint.h>

struct soc_intel_baytrail_config {
        uint8_t sata_port_map;
        uint8_t sata_ahci;
        uint8_t ide_legacy_combined;

	/* USB Port Disable mask */
	uint16_t usb2_port_disable_mask;
	uint16_t usb3_port_disable_mask;

	/* USB routing */
	int usb_route_to_xhci;
};

extern struct chip_operations soc_intel_baytrail_ops;
#endif /* _BAYTRAIL_CHIP_H_ */
