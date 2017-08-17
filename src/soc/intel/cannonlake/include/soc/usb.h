/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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


#ifndef _SOC_USB_H_
#define _SOC_USB_H_

#include <stdint.h>

struct usb2_port_config {
	uint8_t enable;
	uint8_t ocpin;
	uint8_t tx_bias;
	uint8_t tx_emp_enable;
	uint8_t pre_emp_bias;
	uint8_t pre_emp_bit;
};

struct usb3_port_config {
	uint8_t enable;
	uint8_t ocpin;
	uint8_t tx_de_emp;
	uint8_t tx_downscale_amp;
};

#endif
