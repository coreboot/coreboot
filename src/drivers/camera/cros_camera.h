/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __VENDORCODE_GOOGLE_CHROMEOS_CAMERA_H
#define __VENDORCODE_GOOGLE_CHROMEOS_CAMERA_H

#include <stdint.h>

#define CROS_CAMERA_INFO_MAGIC "CrOS"
#define CROS_CAMERA_INFO_VERSION 1
#define CROS_CAMERA_INFO_SIZE_MIN 0x0a

struct cros_camera_info {
	uint8_t magic[4];  /* CROS_CAMERA_INFO_MAGIC */
	uint16_t crc16;
	uint8_t version;
	uint8_t size;
	uint16_t data_format;
	uint16_t module_pid;
	uint8_t module_vid[2];
	uint8_t sensor_vid[2];
	uint16_t sensor_pid;
};

/* Returns 0 on success, non-zero on errors. */
int check_cros_camera_info(const struct cros_camera_info *info);

#endif
