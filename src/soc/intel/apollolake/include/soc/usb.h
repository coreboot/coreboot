/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corp.
 * (Written by Kane Chen <Kane.Chen@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_APOLLOLAKE_USB_H_
#define _SOC_APOLLOLAKE_USB_H_

#define APOLLOLAKE_USB2_PORT_MAX 8

struct usb2_eye_per_port {
	uint8_t Usb20PerPortTxPeHalf;
	uint8_t Usb20PerPortPeTxiSet;
	uint8_t Usb20PerPortTxiSet;
	uint8_t Usb20HsSkewSel;
	uint8_t Usb20IUsbTxEmphasisEn;
	uint8_t Usb20PerPortRXISet;
	uint8_t Usb20HsNpreDrvSel;
};

#endif /* _SOC_APOLLOLAKE_USB_H_ */
