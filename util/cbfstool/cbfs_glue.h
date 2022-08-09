/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CBFS_GLUE_H_
#define _CBFS_GLUE_H_

#include "cbfs_image.h"

#define CBFS_ENABLE_HASHING 1
#define CBFS_HASH_HWCRYPTO 0

typedef const struct cbfs_image *cbfs_dev_t;

static inline ssize_t cbfs_dev_read(cbfs_dev_t dev, void *buffer, size_t offset, size_t size)
{
	if (buffer_size(&dev->buffer) < offset ||
	    buffer_size(&dev->buffer) - offset < size)
		return -1;

	memcpy(buffer, buffer_get(&dev->buffer) + offset, size);
	return size;
}

static inline size_t cbfs_dev_size(cbfs_dev_t dev)
{
	return buffer_size(&dev->buffer);
}

#endif	/* _CBFS_GLUE_H_ */
