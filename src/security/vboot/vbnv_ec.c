/*
 * This file is part of the coreboot project.
 *
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

#include <types.h>
#include <ec/google/chromeec/ec.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/vbnv_layout.h>

void read_vbnv_ec(uint8_t *vbnv_copy)
{
	google_chromeec_vbnv_context(1, vbnv_copy, VBOOT_VBNV_BLOCK_SIZE);
}

void save_vbnv_ec(const uint8_t *vbnv_copy)
{
	google_chromeec_vbnv_context(0, (uint8_t *)vbnv_copy,
				     VBOOT_VBNV_BLOCK_SIZE);
}
