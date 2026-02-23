/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_DEVICE_NVS_H_
#define _BROADWELL_DEVICE_NVS_H_

#include <stdint.h>

#define SIO_NVS_DMA		0
#define SIO_NVS_I2C0		1
#define SIO_NVS_I2C1		2
#define SIO_NVS_SPI0		3
#define SIO_NVS_SPI1		4
#define SIO_NVS_UART0		5
#define SIO_NVS_UART1		6
#define SIO_NVS_SDIO		7
#define SIO_NVS_ADSP		8

struct __packed device_nvs {
	u8	enable[9];
	u32	bar0[9];
	u32	bar1[9];
};

#endif
