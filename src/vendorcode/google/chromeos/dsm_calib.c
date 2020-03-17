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

#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <stdint.h>
#include <string.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

#define DSM_BUF_LEN 128
#define DSM_PREFIX "dsm_calib_"

enum cb_err get_dsm_calibration_from_key(const char *key, uint64_t *value)
{
	static char buf[DSM_BUF_LEN];
	char *ret;
	long value_from_vpd;

	if (strncmp(key, DSM_PREFIX, strlen(DSM_PREFIX))) {
		printk(BIOS_ERR, "got invalid dsm_calib key: %s\n", key);
		return CB_ERR;
	}

	ret = vpd_gets(key, buf, DSM_BUF_LEN, VPD_RO);
	if (!ret) {
		printk(BIOS_ERR, "failed to find key in VPD: %s\n", key);
		return CB_ERR;
	}

	value_from_vpd = atol(buf);
	if (value_from_vpd <= 0) {
		printk(BIOS_ERR, "got invalid dsm_calib from VPD: %ld\n", value_from_vpd);
		return CB_ERR;
	}

	*value = value_from_vpd;

	return CB_SUCCESS;
}
