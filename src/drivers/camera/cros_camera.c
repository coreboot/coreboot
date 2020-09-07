/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <crc_byte.h>
#include <string.h>

#include "cros_camera.h"

int check_cros_camera_info(const struct cros_camera_info *info)
{
	if (memcmp(info->magic, CROS_CAMERA_INFO_MAGIC, sizeof(info->magic))) {
		printk(BIOS_ERR, "Invalid magic in camera info\n");
		return -1;
	}

	const uint8_t *ptr = (void *)(&info->crc16 + 1);
	uint16_t crc16 = 0;
	while (ptr < (uint8_t *)info + sizeof(struct cros_camera_info))
		crc16 = crc16_byte(crc16, *ptr++);

	if (info->crc16 != crc16) {
		printk(BIOS_ERR, "Incorrect CRC16: expected %#06x, got %#06x\n",
		       crc16, info->crc16);
		return -1;
	}

	if (info->version != CROS_CAMERA_INFO_VERSION) {
		printk(BIOS_ERR, "Unknown camera info version: %u\n",
		       info->version);
		return -1;
	}
	if (info->size < CROS_CAMERA_INFO_SIZE_MIN) {
		printk(BIOS_ERR, "Size of camera info is too small: %u\n",
		       info->size);
		return -1;
	}

	return 0;
}
