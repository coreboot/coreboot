/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <drivers/ocp/include/vpd.h>

int get_int_from_vpd_range(const char *const key, const int fallback, const int min,
	const int max)
{
	int val = fallback;

	if (!vpd_get_int(key, VPD_RW_THEN_RO, &val))
		printk(BIOS_INFO, "%s: not able to get VPD %s, default set to %d\n",
		       __func__, key, fallback);
	else
		printk(BIOS_DEBUG, "%s: VPD %s, got %d\n", __func__, key, val);

	if (val < min || val > max) {
		printk(BIOS_INFO, "Invalid VPD %s value, set default value to %d\n",
			key, fallback);
		val = fallback;
	}

	return val;
}

bool get_bool_from_vpd(const char *const key, const bool fallback)
{
	uint8_t val;

	val = (uint8_t)fallback;

	if (!vpd_get_bool(key, VPD_RW_THEN_RO, &val))
		printk(BIOS_INFO, "%s: not able to get VPD %s, default set to %d\n",
		       __func__, key, fallback);
	else
		printk(BIOS_DEBUG, "%s: VPD %s, got %d\n", __func__, key, val);

	return (bool)val;
}
