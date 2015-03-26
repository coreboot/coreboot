/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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
 * Foundation, Inc.
 */

#ifndef __SOC_NVIDIA_TEGRA_USB_H__
#define __SOC_NVIDIA_TEGRA_USB_H__

#include <stdint.h>

enum usb_phy_type {		/* For use in lpm_ctrl[31:29] */
	USB_PHY_UTMIP = 0,
	USB_PHY_ULPI = 2,
	USB_PHY_ICUSB_SER = 3,
	USB_PHY_HSIC = 4,
};

void usb_setup_utmip(void *usb_base);

#endif
