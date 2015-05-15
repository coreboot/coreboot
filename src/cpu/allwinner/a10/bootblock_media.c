/*
 * CBFS accessors for bootblock stage.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */
#include <boot_device.h>
#include <cbfs.h>
#include <console/console.h>

const struct region_device *boot_device_ro(void)
{
	return NULL;
}

int init_default_cbfs_media(struct cbfs_media *media)
{
	printk(BIOS_ERR, "Oh my! I don't know how to access CBFS yet.");
	return -1;
}
