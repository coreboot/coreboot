/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/psp_efs.h>
#include <arch/mmio.h>
#include <types.h>

struct _embedded_firmware *efs = (struct _embedded_firmware *)EFS_ADDRESS;

bool efs_is_valid(void)
{
	if (efs->signature != EMBEDDED_FW_SIGNATURE)
		return false;

	return true;
}

bool read_efs_spi_settings(uint8_t *mode, uint8_t *speed)
{
	if (!efs_is_valid())
		return false;

	*mode = efs->SPI_MODE_FIELD;
	*speed = efs->SPI_SPEED_FIELD;
	return true;
}
