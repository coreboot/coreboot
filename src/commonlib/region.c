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

#include <commonlib/helpers.h>
#include <commonlib/region.h>
#include <string.h>

static inline size_t region_end(const struct region *r)
{
	return region_sz(r) + region_offset(r);
}

static int is_subregion(const struct region *p, const struct region *c)
{
	if (region_offset(c) < region_offset(p))
		return 0;

	if (region_sz(c) > region_sz(p))
		return 0;

	if (region_end(c) > region_end(p))
		return 0;

	return 1;
}

static int normalize_and_ok(const struct region *outer, struct region *inner)
{
	inner->offset += region_offset(outer);
	return is_subregion(outer, inner);
}

static const struct region_device *rdev_root(const struct region_device *rdev)
{
	if (rdev->root == NULL)
		return rdev;
	return rdev->root;
}

ssize_t rdev_relative_offset(const struct region_device *p,
				const struct region_device *c)
{
	if (rdev_root(p) != rdev_root(c))
		return -1;

	if (!is_subregion(&p->region, &c->region))
		return -1;

	return region_device_offset(c) - region_device_offset(p);
}

void *rdev_mmap(const struct region_device *rd, size_t offset, size_t size)
{
	const struct region_device *rdev;
	struct region req = {
		.offset = offset,
		.size = size,
	};

	if (!normalize_and_ok(&rd->region, &req))
		return NULL;

	rdev = rdev_root(rd);

	if (rdev->ops->mmap == NULL)
		return NULL;

	return rdev->ops->mmap(rdev, req.offset, req.size);
}

int rdev_munmap(const struct region_device *rd, void *mapping)
{
	const struct region_device *rdev;

	rdev = rdev_root(rd);

	if (rdev->ops->munmap == NULL)
		return -1;

	return rdev->ops->munmap(rdev, mapping);
}

ssize_t rdev_readat(const struct region_device *rd, void *b, size_t offset,
			size_t size)
{
	const struct region_device *rdev;
	struct region req = {
		.offset = offset,
		.size = size,
	};

	if (!normalize_and_ok(&rd->region, &req))
		return -1;

	rdev = rdev_root(rd);

	return rdev->ops->readat(rdev, b, req.offset, req.size);
}

ssize_t rdev_writeat(const struct region_device *rd, const void *b,
			size_t offset, size_t size)
{
	const struct region_device *rdev;
	struct region req = {
		.offset = offset,
		.size = size,
	};

	if (!normalize_and_ok(&rd->region, &req))
		return -1;

	rdev = rdev_root(rd);

	if (rdev->ops->writeat == NULL)
		return -1;

	return rdev->ops->writeat(rdev, b, req.offset, req.size);
}

ssize_t rdev_eraseat(const struct region_device *rd, size_t offset,
			size_t size)
{
	const struct region_device *rdev;
	struct region req = {
		.offset = offset,
		.size = size,
	};

	if (!normalize_and_ok(&rd->region, &req))
		return -1;

	rdev = rdev_root(rd);

	/* If the eraseat ptr is NULL we assume that the erase
	 * function was completed successfully. */
	if (rdev->ops->eraseat == NULL)
		return size;

	return rdev->ops->eraseat(rdev, req.offset, req.size);
}

int rdev_chain(struct region_device *child, const struct region_device *parent,
		size_t offset, size_t size)
{
	struct region req = {
		.offset = offset,
		.size = size,
	};

	if (!normalize_and_ok(&parent->region, &req))
		return -1;

	/* Keep track of root region device. Note the offsets are relative
	 * to the root device. */
	child->root = rdev_root(parent);
	child->ops = NULL;
	child->region.offset = req.offset;
	child->region.size = req.size;

	return 0;
}

static void mem_region_device_init(struct mem_region_device *mdev,
		const struct region_device_ops *ops, void *base, size_t size)
{
	memset(mdev, 0, sizeof(*mdev));
	mdev->base = base;
	mdev->rdev.ops = ops;
	mdev->rdev.region.size = size;
}

void mem_region_device_ro_init(struct mem_region_device *mdev, void *base,
				size_t size)
{
	return mem_region_device_init(mdev, &mem_rdev_ro_ops, base, size);
}

void mem_region_device_rw_init(struct mem_region_device *mdev, void *base,
		size_t size)
{
	return mem_region_device_init(mdev, &mem_rdev_rw_ops, base, size);
}

void region_device_init(struct region_device *rdev,
			const struct region_device_ops *ops, size_t offset,
			size_t size)
{
	memset(rdev, 0, sizeof(*rdev));
	rdev->root = NULL;
	rdev->ops = ops;
	rdev->region.offset = offset;
	rdev->region.size = size;
}

static void xlate_region_device_init(struct xlate_region_device *xdev,
			const struct region_device_ops *ops,
			const struct region_device *access_dev,
			size_t sub_offset, size_t sub_size,
			size_t parent_size)
{
	memset(xdev, 0, sizeof(*xdev));
	xdev->access_dev = access_dev;
	xdev->sub_region.offset = sub_offset;
	xdev->sub_region.size = sub_size;
	region_device_init(&xdev->rdev, ops, 0, parent_size);
}

void xlate_region_device_ro_init(struct xlate_region_device *xdev,
			      const struct region_device *access_dev,
			      size_t sub_offset, size_t sub_size,
			      size_t parent_size)
{
	xlate_region_device_init(xdev, &xlate_rdev_ro_ops, access_dev,
			sub_offset, sub_size, parent_size);
}

void xlate_region_device_rw_init(struct xlate_region_device *xdev,
			      const struct region_device *access_dev,
			      size_t sub_offset, size_t sub_size,
			      size_t parent_size)
{
	xlate_region_device_init(xdev, &xlate_rdev_rw_ops, access_dev,
			sub_offset, sub_size, parent_size);
}

static void *mdev_mmap(const struct region_device *rd, size_t offset,
			size_t size __unused)
{
	const struct mem_region_device *mdev;

	mdev = container_of(rd, __typeof__(*mdev), rdev);

	return &mdev->base[offset];
}

static int mdev_munmap(const struct region_device * rd __unused,
			void *mapping __unused)
{
	return 0;
}

static ssize_t mdev_readat(const struct region_device *rd, void *b,
				size_t offset, size_t size)
{
	const struct mem_region_device *mdev;

	mdev = container_of(rd, __typeof__(*mdev), rdev);

	memcpy(b, &mdev->base[offset], size);

	return size;
}

static ssize_t mdev_writeat(const struct region_device *rd, const void *b,
				size_t offset, size_t size)
{
	const struct mem_region_device *mdev;

	mdev = container_of(rd, __typeof__(*mdev), rdev);

	memcpy(&mdev->base[offset], b, size);

	return size;
}

static ssize_t mdev_eraseat(const struct region_device *rd, size_t offset,
				size_t size)
{
	const struct mem_region_device *mdev;

	mdev = container_of(rd, __typeof__(*mdev), rdev);

	memset(&mdev->base[offset], 0, size);

	return size;
}

const struct region_device_ops mem_rdev_ro_ops = {
	.mmap = mdev_mmap,
	.munmap = mdev_munmap,
	.readat = mdev_readat,
};

const struct region_device_ops mem_rdev_rw_ops = {
	.mmap = mdev_mmap,
	.munmap = mdev_munmap,
	.readat = mdev_readat,
	.writeat = mdev_writeat,
	.eraseat = mdev_eraseat,
};

void mmap_helper_device_init(struct mmap_helper_region_device *mdev,
				void *cache, size_t cache_size)
{
	mem_pool_init(&mdev->pool, cache, cache_size);
}

void *mmap_helper_rdev_mmap(const struct region_device *rd, size_t offset,
				size_t size)
{
	struct mmap_helper_region_device *mdev;
	void *mapping;

	mdev = container_of((void *)rd, __typeof__(*mdev), rdev);

	mapping = mem_pool_alloc(&mdev->pool, size);

	if (mapping == NULL)
		return NULL;

	if (rd->ops->readat(rd, mapping, offset, size) != size) {
		mem_pool_free(&mdev->pool, mapping);
		return NULL;
	}

	return mapping;
}

int mmap_helper_rdev_munmap(const struct region_device *rd, void *mapping)
{
	struct mmap_helper_region_device *mdev;

	mdev = container_of((void *)rd, __typeof__(*mdev), rdev);

	mem_pool_free(&mdev->pool, mapping);

	return 0;
}

static void *xlate_mmap(const struct region_device *rd, size_t offset,
			size_t size)
{
	const struct xlate_region_device *xldev;
	struct region req = {
		.offset = offset,
		.size = size,
	};

	xldev = container_of(rd, __typeof__(*xldev), rdev);

	if (!is_subregion(&xldev->sub_region, &req))
		return NULL;

	offset -= region_offset(&xldev->sub_region);

	return rdev_mmap(xldev->access_dev, offset, size);
}

static int xlate_munmap(const struct region_device *rd, void *mapping)
{
	const struct xlate_region_device *xldev;

	xldev = container_of(rd, __typeof__(*xldev), rdev);

	return rdev_munmap(xldev->access_dev, mapping);
}

static ssize_t xlate_readat(const struct region_device *rd, void *b,
				size_t offset, size_t size)
{
	struct region req = {
		.offset = offset,
		.size = size,
	};
	const struct xlate_region_device *xldev;

	xldev = container_of(rd, __typeof__(*xldev), rdev);

	if (!is_subregion(&xldev->sub_region, &req))
		return -1;

	offset -= region_offset(&xldev->sub_region);

	return rdev_readat(xldev->access_dev, b, offset, size);
}

static ssize_t xlate_writeat(const struct region_device *rd, const void *b,
				size_t offset, size_t size)
{
	struct region req = {
		.offset = offset,
		.size = size,
	};
	const struct xlate_region_device *xldev;

	xldev = container_of(rd, __typeof__(*xldev), rdev);

	if (!is_subregion(&xldev->sub_region, &req))
		return -1;

	offset -= region_offset(&xldev->sub_region);

	return rdev_writeat(xldev->access_dev, b, offset, size);
}

static ssize_t xlate_eraseat(const struct region_device *rd,
				size_t offset, size_t size)
{
	struct region req = {
		.offset = offset,
		.size = size,
	};
	const struct xlate_region_device *xldev;

	xldev = container_of(rd, __typeof__(*xldev), rdev);

	if (!is_subregion(&xldev->sub_region, &req))
		return -1;

	offset -= region_offset(&xldev->sub_region);

	return rdev_eraseat(xldev->access_dev, offset, size);
}

const struct region_device_ops xlate_rdev_ro_ops = {
	.mmap = xlate_mmap,
	.munmap = xlate_munmap,
	.readat = xlate_readat,
};

const struct region_device_ops xlate_rdev_rw_ops = {
	.mmap = xlate_mmap,
	.munmap = xlate_munmap,
	.readat = xlate_readat,
	.writeat = xlate_writeat,
	.eraseat = xlate_eraseat,
};
