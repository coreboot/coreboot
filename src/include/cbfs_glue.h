/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CBFS_GLUE_H_
#define _CBFS_GLUE_H_

#include <commonlib/region.h>
#include <console/console.h>
#include <security/vboot/misc.h>
#include <rules.h>

/*
 * This flag prevents linking hashing functions into stages where they're not required. We don't
 * need them at all if verification is disabled. If verification is enabled without TOCTOU
 * safety, we only need to verify the metadata hash in the initial stage and can assume it stays
 * valid in later stages. If TOCTOU safety is required, we may need them in every stage to
 * reverify metadata that had to be reloaded from flash (e.g. because it didn't fit the mcache).
 * Note that this only concerns metadata hashing -- file access functions may still link hashing
 * routines independently for file data hashing.
 */
#define CBFS_ENABLE_HASHING (CONFIG(CBFS_VERIFICATION) && \
			     (CONFIG(TOCTOU_SAFETY) || ENV_INITIAL_STAGE))
#define CBFS_HASH_HWCRYPTO vboot_hwcrypto_allowed()

#define ERROR(...) printk(BIOS_ERR, "CBFS ERROR: " __VA_ARGS__)
#define LOG(...) printk(BIOS_INFO, "CBFS: " __VA_ARGS__)
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
