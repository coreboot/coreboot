/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>

const struct region_device *boot_device_ro(void)
{
	/* FIXME: add support for reading coreboot from NAND */
	return NULL;
}
