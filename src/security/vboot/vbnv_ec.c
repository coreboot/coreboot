/* SPDX-License-Identifier: GPL-2.0-only */

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
