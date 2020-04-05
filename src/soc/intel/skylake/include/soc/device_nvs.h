/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SOC_DEVICE_NVS_H_
#define _SOC_DEVICE_NVS_H_

#include <stdint.h>

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
