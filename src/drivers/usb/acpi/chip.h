/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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

#ifndef __USB_ACPI_CHIP_H__
#define __USB_ACPI_CHIP_H__

#include <arch/acpi.h>
#include <arch/acpi_pld.h>

struct drivers_usb_acpi_config {
	const char *desc;

	/*
	 * Physical ports that are user visible
	 *
	 * UPC_TYPE_A
	 * UPC_TYPE_MINI_AB
	 * UPC_TYPE_EXPRESSCARD
	 * UPC_TYPE_USB3_A
	 * UPC_TYPE_USB3_B
	 * UPC_TYPE_USB3_MICRO_B
	 * UPC_TYPE_USB3_MICRO_AB
	 * UPC_TYPE_USB3_POWER_B
	 * UPC_TYPE_C_USB2_ONLY
	 * UPC_TYPE_C_USB2_SS_SWITCH
	 * UPC_TYPE_C_USB2_SS
	 *
	 * Non-visible ports or special devices
	 *
	 * UPC_TYPE_PROPRIETARY
	 * UPC_TYPE_INTERNAL
	 * UPC_TYPE_UNUSED
	 * UPC_TYPE_HUB
	 */
	enum acpi_upc_type type;

	/* Define a custom physical location for the port */
	bool use_custom_pld;
	struct acpi_pld custom_pld;
};

#endif /* __USB_ACPI_CHIP_H__ */
