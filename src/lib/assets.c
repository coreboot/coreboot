/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */


#include <assets.h>
#include <boot_device.h>
#include <cbfs.h>
#include <console/console.h>
#include <rules.h>
#include <stdlib.h>

#define DEFAULT_CBFS_PROVIDER_PRESENT \
	(!ENV_VERSTAGE || (ENV_VERSTAGE && !CONFIG_RETURN_FROM_VERSTAGE))

#if DEFAULT_CBFS_PROVIDER_PRESENT
static int cbfs_boot_asset_locate(struct asset *asset)
{
	struct cbfsf file;

	if (cbfs_boot_locate(&file, asset_name(asset), NULL))
		return -1;

	cbfs_file_data(asset_rdev(asset), &file);

	return 0;
}

static const struct asset_provider cbfs_default_provider = {
	.name = "CBFS",
	.locate = cbfs_boot_asset_locate,
};
#endif

extern const struct asset_provider vboot_provider;

static const struct asset_provider *providers[] = {
#if CONFIG_VBOOT_VERIFY_FIRMWARE
	&vboot_provider,
#endif
#if DEFAULT_CBFS_PROVIDER_PRESENT
	&cbfs_default_provider,
#endif
};

int asset_locate(struct asset *asset)
{
	int i;

	boot_device_init();

	for (i = 0; i < ARRAY_SIZE(providers); i++) {
		/* Default provider state is active. */
		int ret = 1;
		const struct asset_provider *ops;

		ops = providers[i];

		if (ops->is_active != NULL)
			ret = ops->is_active(asset);

		if (ret == 0) {
			printk(BIOS_DEBUG, "%s provider inactive.\n",
				ops->name);
			continue;
		} else if (ret < 0) {
			printk(BIOS_DEBUG, "%s provider failure.\n",
				ops->name);
			continue;
		}

		printk(BIOS_DEBUG, "%s provider active.\n", ops->name);

		if (ops->locate(asset))
			continue;

		printk(BIOS_DEBUG, "'%s' located at offset: %zx size: %zx\n",
			asset->name, region_device_offset(&asset->rdev),
			region_device_sz(&asset->rdev));

		return 0;
	}

	return -1;
}
