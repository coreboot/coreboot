/* SPDX-License-Identifier: GPL-2.0-only */

#include <sar.h>
#include <chip.h>
#include <fw_config.h>
#include <baseboard/variants.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return "wifi_sar_0.hex";
}
