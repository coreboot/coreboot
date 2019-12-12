/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CBFS_GLUE_H_
#define _CBFS_GLUE_H_

#include <commonlib/region.h>
#include <console/console.h>

#define CBFS_ENABLE_HASHING 0

#define ERROR(...) printk(BIOS_ERR, "CBFS ERROR: " __VA_ARGS__)
#define LOG(...) printk(BIOS_ERR, "CBFS: " __VA_ARGS__)
#define DEBUG(...) do { \
	if (CONFIG(DEBUG_CBFS)) \
		printk(BIOS_SPEW, "CBFS DEBUG: " __VA_ARGS__); \
} while (0)

typedef const struct region_device *cbfs_dev_t;

static inline ssize_t cbfs_dev_read(cbfs_dev_t dev, void *buffer, size_t offset, size_t size)
{
	return rdev_readat(dev, buffer, offset, size);
}

static inline size_t cbfs_dev_size(cbfs_dev_t dev)
{
	return region_device_sz(dev);
}

#endif	/* _CBFS_GLUE_H_ */
