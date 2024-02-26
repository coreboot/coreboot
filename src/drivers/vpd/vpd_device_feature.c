/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/vpd/vpd.h>

#define VPD_KEY_FEATURE_DEVICE_INFO	"feature_device_info"
#define VPD_FEATURE_DEVICE_INFO_LEN	64

const char *vpd_get_feature_device_info(void)
{
	static char device_info[VPD_FEATURE_DEVICE_INFO_LEN];
	if (vpd_gets(VPD_KEY_FEATURE_DEVICE_INFO, device_info, VPD_FEATURE_DEVICE_INFO_LEN,
			 VPD_RW))
		return device_info;
	return "";
}
