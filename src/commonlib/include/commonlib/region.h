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
 */

#ifndef _REGION_H_
#define _REGION_H_

#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>
#include <commonlib/mem_pool.h>

/*
 * Region support.
 *
 * Regions are intended to abstract away the access mechanisms for blocks of
 * data. This could be SPI, eMMC, or a memory region as the backing store.
 * They are accessed through a region_device.  Subregions can be made by
 * chaining together multiple region_devices.
 */

struct region_device;

/*
 * Returns NULL on error otherwise a buffer is returned with the conents of
 * the requested data at offset of size.
 */
void *rdev_mmap(const struct region_device *rd, size_t offset, size_t size);

/* Unmap a previously mapped area. Returns 0 on success, < 0 on error. */
int rdev_munmap(const struct region_device *rd, void *mapping);

/*
 * Returns < 0 on error otherwise returns size of data read at provided
 * offset filling in the buffer passed.
 */
ssize_t rdev_readat(const struct region_device *rd, void *b, size_t offset,
			size_t size);

/*
 * Returns < 0 on error otherwise returns size of data wrote at provided
 * offset from the buffer passed.
 */
ssize_t rdev_writeat(const struct region_device *rd, const void *b,
			size_t offset, size_t size);

/*
 * Returns < 0 on error otherwise returns size of data erased.
 * If eraseat ops is not defined it returns size which indicates
 * that operation was successful.
 */
ssize_t rdev_eraseat(const struct region_device *rd, size_t offset,
			size_t size);

/****************************************
 *  Implementation of a region device   *
 ****************************************/

/*
 * Create a child region of the parent provided the sub-region is within
 * the parent's region. Returns < 0 on error otherwise 0 on success. Note
 * that the child device only calls through the parent's operations.
 */
int rdev_chain(struct region_device *child, const struct region_device *parent,
		size_t offset, size_t size);


/* A region_device operations. */
struct region_device_ops {
	void *(*mmap)(const struct region_device *, size_t, size_t);
	int (*munmap)(const struct region_device *, void *);
	ssize_t (*readat)(const struct region_device *, void *, size_t, size_t);
	ssize_t (*writeat)(const struct region_device *, const void *, size_t,
		size_t);
	ssize_t (*eraseat)(const struct region_device *, size_t, size_t);
};

struct region {
	size_t offset;
	size_t size;
};

struct region_device {
	const struct region_device *root;
	const struct region_device_ops *ops;
	struct region region;
};

#define REGION_DEV_INIT(ops_, offset_, size_)		\
	{						\
		.root = NULL,				\
		.ops = (ops_),				\
		.region = {				\
			.offset = (offset_),		\
			.size = (size_),		\
		},					\
	}

/* Helper to dynamically initialize region device. */
void region_device_init(struct region_device *rdev,
			const struct region_device_ops *ops, size_t offset,
			size_t size);

static inline size_t region_offset(const struct region *r)
{
	return r->offset;
}

static inline size_t region_sz(const struct region *r)
{
	return r->size;
}

static inline const struct region *region_device_region(
					const struct region_device *rdev)
{
	return &rdev->region;
}

static inline size_t region_device_sz(const struct region_device *rdev)
{
	return region_sz(region_device_region(rdev));
}

static inline size_t region_device_offset(const struct region_device *rdev)
{
	return region_offset(region_device_region(rdev));
}

/* Memory map entire region device. Same semantics as rdev_mmap() above. */
static inline void *rdev_mmap_full(const struct region_device *rd)
{
	return rdev_mmap(rd, 0, region_device_sz(rd));
}

/*
 * Compute relative offset of the child (c) w.r.t. the parent (p). Returns < 0
 * when child is not within the parent's region.
 */
ssize_t rdev_relative_offset(const struct region_device *p,
				const struct region_device *c);

struct mem_region_device {
	char *base;
	struct region_device rdev;
};

/* Inititalize at runtime a mem_region_device. This would be used when
 * the base and size are dynamic or can't be known during linking.
 * There are two variants: read-only and read-write. */
void mem_region_device_ro_init(struct mem_region_device *mdev, void *base,
				size_t size);

void mem_region_device_rw_init(struct mem_region_device *mdev, void *base,
				size_t size);

extern const struct region_device_ops mem_rdev_ro_ops;

extern const struct region_device_ops mem_rdev_rw_ops;

/* Statically initialize mem_region_device. */
#define MEM_REGION_DEV_INIT(base_, size_, ops_)				\
	{								\
		.base = (void *)(base_),				\
		.rdev = REGION_DEV_INIT((ops_), 0, (size_)),		\
	}

#define MEM_REGION_DEV_RO_INIT(base_, size_)				\
		MEM_REGION_DEV_INIT(base_, size_, &mem_rdev_ro_ops)	\

#define MEM_REGION_DEV_RW_INIT(base_, size_)				\
		MEM_REGION_DEV_INIT(base_, size_, &mem_rdev_rw_ops)	\

struct mmap_helper_region_device {
	struct mem_pool pool;
	struct region_device rdev;
};

#define MMAP_HELPER_REGION_INIT(ops_, offset_, size_)			\
	{								\
		.rdev = REGION_DEV_INIT((ops_), (offset_), (size_)),	\
	}

void mmap_helper_device_init(struct mmap_helper_region_device *mdev,
				void *cache, size_t cache_size);

void *mmap_helper_rdev_mmap(const struct region_device *, size_t, size_t);
int mmap_helper_rdev_munmap(const struct region_device *, void *);

/* A translated region device provides the ability to publish a region device
 * in one address space and use an access mechanism within another address
 * space. The sub region is the window within the 1st address space and
 * the request is modified prior to accessing the second address space
 * provided by access_dev. */
struct xlate_region_device {
	const struct region_device *access_dev;
	struct region sub_region;
	struct region_device rdev;
};

extern const struct region_device_ops xlate_rdev_ro_ops;

extern const struct region_device_ops xlate_rdev_rw_ops;

#define XLATE_REGION_DEV_INIT(access_dev_, sub_offset_, sub_size_,	\
		parent_sz_, ops_)					\
	{								\
		.access_dev = access_dev_,				\
		.sub_region = {						\
			.offset = (sub_offset_),			\
			.size = (sub_size_),				\
		},							\
		.rdev = REGION_DEV_INIT((ops_), 0,  (parent_sz_)),	\
	}

#define XLATE_REGION_DEV_RO_INIT(access_dev_, sub_offset_, sub_size_,	\
		parent_sz_)						\
		XLATE_REGION_DEV_INIT(access_dev_, sub_offset_,		\
			sub_size_, parent_sz_, &xlate_rdev_ro_ops),	\

#define XLATE_REGION_DEV_RW_INIT(access_dev_, sub_offset_, sub_size_,	\
		parent_sz_)						\
		XLATE_REGION_DEV_INIT(access_dev_, sub_offset_,		\
			sub_size_, parent_sz_, &xlate_rdev_rw_ops),	\

/* Helper to dynamically initialize xlate region device. */
void xlate_region_device_ro_init(struct xlate_region_device *xdev,
			      const struct region_device *access_dev,
			      size_t sub_offset, size_t sub_size,
			      size_t parent_size);

void xlate_region_device_rw_init(struct xlate_region_device *xdev,
			      const struct region_device *access_dev,
			      size_t sub_offset, size_t sub_size,
			      size_t parent_size);

#endif /* _REGION_H_ */
