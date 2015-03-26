/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

#include <types.h>
#include <string.h>
#include <console/console.h>
#include <arch/early_variables.h>
#include <ec/google/chromeec/ec.h>
#include "chromeos.h"
#include "vbnv_layout.h"

static int vbnv_initialized CAR_GLOBAL;
static uint8_t vbnv[VBNV_BLOCK_SIZE] CAR_GLOBAL;

/* Wrappers for accessing the variables marked as CAR_GLOBAL. */
static inline int is_vbnv_initialized(void)
{
	return car_get_var(vbnv_initialized);
}

static inline uint8_t *vbnv_data_addr(int index)
{
	uint8_t *vbnv_arr = car_get_var_ptr(vbnv);

	return &vbnv_arr[index];
}

static inline uint8_t vbnv_data(int index)
{
	return *vbnv_data_addr(index);
}

/* Return CRC-8 of the data, using x^8 + x^2 + x + 1 polynomial.  A
 * table-based algorithm would be faster, but for only 15 bytes isn't
 * worth the code size.
 */

static uint8_t crc8(const uint8_t * data, int len)
{
	unsigned crc = 0;
	int i, j;

	for (j = len; j; j--, data++) {
		crc ^= (*data << 8);
		for (i = 8; i; i--) {
			if (crc & 0x8000)
				crc ^= (0x1070 << 3);
			crc <<= 1;
		}
	}

	return (uint8_t) (crc >> 8);
}

void read_vbnv(uint8_t *vbnv_copy)
{
	google_chromeec_vbnv_context(1, vbnv_copy, VBNV_BLOCK_SIZE);

	/* Check data for consistency */
	if ((HEADER_SIGNATURE != (vbnv_copy[HEADER_OFFSET] & HEADER_MASK))
	    || (crc8(vbnv_copy, CRC_OFFSET) != vbnv_copy[CRC_OFFSET])) {

		/* Data is inconsistent (bad CRC or header),
		 * so reset to defaults
		 */
		memset(vbnv_copy, 0, VBNV_BLOCK_SIZE);
		vbnv_copy[HEADER_OFFSET] =
		    (HEADER_SIGNATURE | HEADER_FIRMWARE_SETTINGS_RESET |
		     HEADER_KERNEL_SETTINGS_RESET);
	}
}

void save_vbnv(const uint8_t *vbnv_copy)
{
	google_chromeec_vbnv_context(0, (uint8_t *)vbnv_copy, VBNV_BLOCK_SIZE);
}

static void vbnv_setup(void)
{
	read_vbnv(vbnv_data_addr(0));
	car_set_var(vbnv_initialized, 1);
}

int get_recovery_mode_from_vbnv(void)
{
	if (!is_vbnv_initialized())
		vbnv_setup();
	return vbnv_data(RECOVERY_OFFSET);
}

int vboot_wants_oprom(void)
{
	if (!is_vbnv_initialized())
		vbnv_setup();

	return (vbnv_data(BOOT_OFFSET) & BOOT_OPROM_NEEDED) ? 1 : 0;
}
