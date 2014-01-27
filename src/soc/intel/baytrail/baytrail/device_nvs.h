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

#include <stdint.h>

#define LPSS_NVS_SIO_DMA1	0
#define LPSS_NVS_I2C1		1
#define LPSS_NVS_I2C2		2
#define LPSS_NVS_I2C3		3
#define LPSS_NVS_I2C4		4
#define LPSS_NVS_I2C5		5
#define LPSS_NVS_I2C6		6
#define LPSS_NVS_I2C7		7
#define LPSS_NVS_SIO_DMA2	8
#define LPSS_NVS_SPI		9
#define LPSS_NVS_PWM1		10
#define LPSS_NVS_PWM2		11
#define LPSS_NVS_HSUART1	12
#define LPSS_NVS_HSUART2	13

#define SCC_NVS_MMC		0
#define SCC_NVS_SDIO		1
#define SCC_NVS_SD		2

typedef struct {
	/* Device Enabled in ACPI Mode */
	u8	lpss_en[14];
	u8	scc_en[3];
	u8	lpe_en;

	/* BAR 0 */
	u32	lpss_bar0[14];
	u32	scc_bar0[3];
	u32	lpe_bar0;

	/* BAR 0 */
	u32	lpss_bar1[14];
	u32	scc_bar1[3];
	u32	lpe_bar1;

	/* Extra */
	u32	lpe_fw; /* LPE Firmware */
} __attribute__((packed)) device_nvs_t;
