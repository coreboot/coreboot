/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <boot_device.h>

void __weak boot_device_init(void)
{
	/* Provide weak do-nothing init. */
}

int __weak boot_device_wp_region(const struct region_device *rd,
				 const enum bootdev_prot_type type)
{
	/* return a failure, make aware WP is not implemented */
	return -1;
}

static int boot_device_subregion(const struct region *sub,
				struct region_device *subrd,
				const struct region_device *parent)
{
	if (parent == NULL)
		return -1;

	return rdev_chain(subrd, parent, region_offset(sub), region_sz(sub));
}

int boot_device_ro_subregion(const struct region *sub,
				struct region_device *subrd)
{
	/* Ensure boot device has been initialized at least once. */
	boot_device_init();

	return boot_device_subregion(sub, subrd, boot_device_ro());
}

int boot_device_rw_subregion(const struct region *sub,
				struct region_device *subrd)
{
	/* Ensure boot device has been initialized at least once. */
	boot_device_init();

	return boot_device_subregion(sub, subrd, boot_device_rw());
}
