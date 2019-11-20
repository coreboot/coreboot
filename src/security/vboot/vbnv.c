/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
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

#include <string.h>
#include <types.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/vbnv_layout.h>

static int vbnv_initialized;
static uint8_t vbnv[VBOOT_VBNV_BLOCK_SIZE];

/* Return CRC-8 of the data, using x^8 + x^2 + x + 1 polynomial. */
static uint8_t crc8_vbnv(const uint8_t *data, int len)
{
	unsigned int crc = 0;
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

void vbnv_reset(uint8_t *vbnv_copy)
{
	memset(vbnv_copy, 0, VBOOT_VBNV_BLOCK_SIZE);
}

/* Read VBNV data into cache. */
static void vbnv_setup(void)
{
	if (!vbnv_initialized) {
		read_vbnv(vbnv);
		vbnv_initialized = 1;
	}
}

/* Verify VBNV header and checksum. */
int verify_vbnv(uint8_t *vbnv_copy)
{
	return (HEADER_SIGNATURE == (vbnv_copy[HEADER_OFFSET] & HEADER_MASK)) &&
		(crc8_vbnv(vbnv_copy, CRC_OFFSET) == vbnv_copy[CRC_OFFSET]);
}

/* Re-generate VBNV checksum. */
void regen_vbnv_crc(uint8_t *vbnv_copy)
{
	vbnv_copy[CRC_OFFSET] = crc8_vbnv(vbnv_copy, CRC_OFFSET);
}

/*
 * Read VBNV data from configured storage backend.
 * If VBNV verification fails, reset the vbnv copy.
 */
void read_vbnv(uint8_t *vbnv_copy)
{
	if (CONFIG(VBOOT_VBNV_CMOS))
		read_vbnv_cmos(vbnv_copy);
	else if (CONFIG(VBOOT_VBNV_EC))
		read_vbnv_ec(vbnv_copy);
	else if (CONFIG(VBOOT_VBNV_FLASH))
		read_vbnv_flash(vbnv_copy);

	/* Check data for consistency */
	if (!verify_vbnv(vbnv_copy))
		vbnv_reset(vbnv_copy);
}

/*
 * Write VBNV data to configured storage backend.
 * This assumes that the caller has updated the CRC already.
 */
void save_vbnv(const uint8_t *vbnv_copy)
{
	if (CONFIG(VBOOT_VBNV_CMOS))
		save_vbnv_cmos(vbnv_copy);
	else if (CONFIG(VBOOT_VBNV_EC))
		save_vbnv_ec(vbnv_copy);
	else if (CONFIG(VBOOT_VBNV_FLASH))
		save_vbnv_flash(vbnv_copy);

	/* Clear initialized flag to force cached data to be updated */
	vbnv_initialized = 0;
}

/* Save a recovery reason into VBNV. */
void set_recovery_mode_into_vbnv(int recovery_reason)
{
	uint8_t vbnv_copy[VBOOT_VBNV_BLOCK_SIZE];

	read_vbnv(vbnv_copy);

	vbnv_copy[RECOVERY_OFFSET] = recovery_reason;
	vbnv_copy[CRC_OFFSET] = crc8_vbnv(vbnv_copy, CRC_OFFSET);

	save_vbnv(vbnv_copy);
}

/* Read the recovery reason from VBNV. */
int get_recovery_mode_from_vbnv(void)
{
	vbnv_setup();
	return vbnv[RECOVERY_OFFSET];
}

/* Read the USB Device Controller(UDC) enable flag from VBNV. */
int vbnv_udc_enable_flag(void)
{
	vbnv_setup();
	return (vbnv[DEV_FLAGS_OFFSET] & DEV_ENABLE_UDC) ? 1 : 0;
}

void vbnv_init(uint8_t *vbnv_copy)
{
	if (CONFIG(VBOOT_VBNV_CMOS))
		vbnv_init_cmos(vbnv_copy);
	read_vbnv(vbnv_copy);
}
