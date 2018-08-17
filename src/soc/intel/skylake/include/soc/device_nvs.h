/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#ifndef _SOC_DEVICE_NVS_H_
#define _SOC_DEVICE_NVS_H_

#include <stdint.h>
#include <compiler.h>

#define SIO_NVS_I2C0		0
#define SIO_NVS_I2C1		1
#define SIO_NVS_I2C2		2
#define SIO_NVS_I2C3		3
#define SIO_NVS_I2C4		4
#define SIO_NVS_I2C5		5
#define SIO_NVS_SPI0		6
#define SIO_NVS_SPI1		7
#define SIO_NVS_UART0		8
#define SIO_NVS_UART1		9
#define SIO_NVS_UART2		10

typedef struct {
	u8	enable[11];
	u32	bar0[11];
	u32	bar1[11];
} __packed device_nvs_t;

#endif
