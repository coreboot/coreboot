/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _CBFS_CBFS_GLUE_H
#define _CBFS_CBFS_GLUE_H

#include <libpayload-config.h>
#include <boot_device.h>
#include <stdbool.h>
#include <stdio.h>

#define CBFS_ENABLE_HASHING CONFIG(LP_CBFS_VERIFICATION)
#define CBFS_HASH_HWCRYPTO cbfs_hwcrypto_allowed()

#define ERROR(...) printf("CBFS ERROR: " __VA_ARGS__)
#define LOG(...) printf("CBFS: " __VA_ARGS__)
#define DEBUG(...)                                                                             \
	do {                                                                                   \
		if (CONFIG(LP_DEBUG_CBFS))                                                     \
			printf("CBFS DEBUG: " __VA_ARGS__);                                    \
	} while (0)

struct cbfs_dev {
	size_t offset;
	size_t size;
};

struct cbfs_boot_device {
	struct cbfs_dev dev;
	void *mcache;
	size_t mcache_size;
};

typedef const struct cbfs_dev *cbfs_dev_t;

static inline ssize_t cbfs_dev_read(cbfs_dev_t dev, void *buffer, size_t offset, size_t size)
{
	if (offset + size < offset || offset + size > dev->size)
		return CB_ERR_ARG;

	return boot_device_read(buffer, dev->offset + offset, size);
}

static inline size_t cbfs_dev_size(cbfs_dev_t dev)
{
	return dev->size;
}

bool cbfs_hwcrypto_allowed(void);

#endif /* _CBFS_CBFS_GLUE_H */
