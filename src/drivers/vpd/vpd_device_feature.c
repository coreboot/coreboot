/* SPDX-License-Identifier: GPL-2.0-only */

#include <b64_decode.h>
#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <stdlib.h>

#define VPD_KEY_FEATURE_DEVICE_INFO	"feature_device_info"

/*
 * Extracts the "feature_level" from the "feature_device_info" VPD key.
 * This key holds a base64-encoded protobuf where "feature_level" is the first entry.
 */
uint8_t vpd_get_feature_level(void)
{
	const uint8_t *device_info;
	int device_info_size, feature_level = 0;
	uint8_t *decoded_device_info;
	size_t decoded_size;

	device_info = vpd_find(VPD_KEY_FEATURE_DEVICE_INFO, &device_info_size, VPD_RW);
	if (!device_info)
		return feature_level;

	decoded_size = B64_DECODED_SIZE(device_info_size);
	decoded_device_info = malloc(decoded_size);
	if (!decoded_device_info) {
		printk(BIOS_ERR, "%s: failed allocating %zd bytes\n", __func__, decoded_size);
		return feature_level;
	}

	/* The index 1 of the decoded data is the "feature level" value */
	if (b64_decode(device_info, device_info_size, decoded_device_info))
		feature_level = decoded_device_info[1];

	free(decoded_device_info);
	return feature_level;
}
