/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <commonlib/region.h>
#include <stdint.h>
#include <string.h>

int region_is_subregion(const struct region *p, const struct region *c)
{
	if (region_offset(c) < region_offset(p))
		return 0;

	if (region_end(c) > region_end(p))
		return 0;

	if (region_end(c) < region_offset(c))
		return 0;

	return 1;
}

static int normalize_and_ok(const struct region *outer, struct region *inner)
{
	inner->offset += region_offset(outer);
	return region_is_subregion(outer, inner);
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

	if (!region_is_subregion(&p->region, &c->region))
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
			size_t window_count, const struct xlate_window *window_arr,
			size_t parent_size)
{
	memset(xdev, 0, sizeof(*xdev));
	xdev->window_count = window_count;
	xdev->window_arr = window_arr;
	region_device_init(&xdev->rdev, ops, 0, parent_size);
}

void xlate_region_device_ro_init(struct xlate_region_device *xdev,
			      size_t window_count, const struct xlate_window *window_arr,
			      size_t parent_size)
{
	xlate_region_device_init(xdev, &xlate_rdev_ro_ops, window_count, window_arr,
			parent_size);
}

void xlate_region_device_rw_init(struct xlate_region_device *xdev,
			      size_t window_count, const struct xlate_window *window_arr,
			      size_t parent_size)
{
	xlate_region_device_init(xdev, &xlate_rdev_rw_ops, window_count, window_arr,
			parent_size);
}

void xlate_window_init(struct xlate_window *window, const struct region_device *access_dev,
			      size_t sub_region_offset, size_t sub_region_size)
{
	window->access_dev = access_dev;
	window->sub_region.offset = sub_region_offset;
	window->sub_region.size = sub_region_size;
}

static void *mdev_mmap(const struct region_device *rd, size_t offset,
			size_t size __always_unused)
{
	const struct mem_region_device *mdev;

	mdev = container_of(rd, __typeof__(*mdev), rdev);

	return &mdev->base[offset];
}

static int mdev_munmap(const struct region_device *rd __always_unused,
			void *mapping __always_unused)
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

static const struct mem_region_device mem_rdev = MEM_REGION_DEV_RO_INIT(0, ~(size_t)0);
static const struct mem_region_device mem_rdev_rw = MEM_REGION_DEV_RW_INIT(0, ~(size_t)0);

int rdev_chain_mem(struct region_device *child, const void *base, size_t size)
{
	return rdev_chain(child, &mem_rdev.rdev, (uintptr_t)base, size);
}

int rdev_chain_mem_rw(struct region_device *child, void *base, size_t size)
{
	return rdev_chain(child, &mem_rdev_rw.rdev, (uintptr_t)base, size);
}

void *mmap_helper_rdev_mmap(const struct region_device *rd, size_t offset,
				size_t size)
{
	struct mmap_helper_region_device *mdev;
	void *mapping;

	mdev = container_of((void *)rd, __typeof__(*mdev), rdev);

	mapping = mem_pool_alloc(mdev->pool, size);

	if (mapping == NULL)
		return NULL;

	if (rd->ops->readat(rd, mapping, offset, size) != size) {
		mem_pool_free(mdev->pool, mapping);
		return NULL;
	}

	return mapping;
}

int mmap_helper_rdev_munmap(const struct region_device *rd, void *mapping)
{
	struct mmap_helper_region_device *mdev;

	mdev = container_of((void *)rd, __typeof__(*mdev), rdev);

	mem_pool_free(mdev->pool, mapping);

	return 0;
}

static const struct xlate_window *xlate_find_window(const struct xlate_region_device *xldev,
						    const struct region *req)
{
	size_t i;
	const struct xlate_window *xlwindow;

	for (i = 0; i < xldev->window_count; i++) {
		xlwindow = &xldev->window_arr[i];
		if (region_is_subregion(&xlwindow->sub_region, req))
			return xlwindow;
	}

	return NULL;
}

static void *xlate_mmap(const struct region_device *rd, size_t offset,
			size_t size)
{
	const struct xlate_region_device *xldev;
	struct region req = {
		.offset = offset,
		.size = size,
	};
	const struct xlate_window *xlwindow;

	xldev = container_of(rd, __typeof__(*xldev), rdev);

	xlwindow = xlate_find_window(xldev, &req);
	if (!xlwindow)
		return NULL;

	offset -= region_offset(&xlwindow->sub_region);

	return rdev_mmap(xlwindow->access_dev, offset, size);
}

static int xlate_munmap(const struct region_device *rd __always_unused,
			void *mapping __always_unused)
{
	/*
	 * xlate_region_device does not keep track of the access device that was used to service
	 * a mmap request. So, munmap does not do anything. If munmap functionality is required,
	 * then xlate_region_device will have to be updated to accept some pre-allocated space
	 * from caller to keep track of the mapping requests. Since xlate_region_device is only
	 * used for memory mapped boot media on the backend right now, skipping munmap is fine.
	 */
	return 0;
}

static ssize_t xlate_readat(const struct region_device *rd, void *b,
				size_t offset, size_t size)
{
	struct region req = {
		.offset = offset,
		.size = size,
	};
	const struct xlate_window *xlwindow;
	const struct xlate_region_device *xldev;

	xldev = container_of(rd, __typeof__(*xldev), rdev);

	xlwindow = xlate_find_window(xldev, &req);
	if (!xlwindow)
		return -1;

	offset -= region_offset(&xlwindow->sub_region);

	return rdev_readat(xlwindow->access_dev, b, offset, size);
}

static ssize_t xlate_writeat(const struct region_device *rd, const void *b,
				size_t offset, size_t size)
{
	struct region req = {
		.offset = offset,
		.size = size,
	};
	const struct xlate_window *xlwindow;
	const struct xlate_region_device *xldev;

	xldev = container_of(rd, __typeof__(*xldev), rdev);

	xlwindow = xlate_find_window(xldev, &req);
	if (!xlwindow)
		return -1;

	offset -= region_offset(&xlwindow->sub_region);

	return rdev_writeat(xlwindow->access_dev, b, offset, size);
}

static ssize_t xlate_eraseat(const struct region_device *rd,
				size_t offset, size_t size)
{
	struct region req = {
		.offset = offset,
		.size = size,
	};
	const struct xlate_window *xlwindow;
	const struct xlate_region_device *xldev;

	xldev = container_of(rd, __typeof__(*xldev), rdev);

	xlwindow = xlate_find_window(xldev, &req);
	if (!xlwindow)
		return -1;

	offset -= region_offset(&xlwindow->sub_region);

	return rdev_eraseat(xlwindow->access_dev, offset, size);
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

static void *incoherent_mmap(const struct region_device *rd, size_t offset,
				size_t size)
{
	const struct incoherent_rdev *irdev;

	irdev = container_of(rd, const struct incoherent_rdev, rdev);

	return rdev_mmap(irdev->read, offset, size);
}

static int incoherent_munmap(const struct region_device *rd, void *mapping)
{
	const struct incoherent_rdev *irdev;

	irdev = container_of(rd, const struct incoherent_rdev, rdev);

	return rdev_munmap(irdev->read, mapping);
}

static ssize_t incoherent_readat(const struct region_device *rd, void *b,
				size_t offset, size_t size)
{
	const struct incoherent_rdev *irdev;

	irdev = container_of(rd, const struct incoherent_rdev, rdev);

	return rdev_readat(irdev->read, b, offset, size);
}

static ssize_t incoherent_writeat(const struct region_device *rd, const void *b,
			size_t offset, size_t size)
{
	const struct incoherent_rdev *irdev;

	irdev = container_of(rd, const struct incoherent_rdev, rdev);

	return rdev_writeat(irdev->write, b, offset, size);
}

static ssize_t incoherent_eraseat(const struct region_device *rd, size_t offset,
				size_t size)
{
	const struct incoherent_rdev *irdev;

	irdev = container_of(rd, const struct incoherent_rdev, rdev);

	return rdev_eraseat(irdev->write, offset, size);
}

static const struct region_device_ops incoherent_rdev_ops = {
	.mmap = incoherent_mmap,
	.munmap = incoherent_munmap,
	.readat = incoherent_readat,
	.writeat = incoherent_writeat,
	.eraseat = incoherent_eraseat,
};

const struct region_device *incoherent_rdev_init(struct incoherent_rdev *irdev,
				const struct region *r,
				const struct region_device *read,
				const struct region_device *write)
{
	const size_t size = region_sz(r);

	if (size != region_device_sz(read) || size != region_device_sz(write))
		return NULL;

	/* The region is represented as offset 0 to size. That way, the generic
	 * rdev operations can be called on the read or write implementation
	 * without any unnecessary translation because the offsets all start
	 * at 0. */
	region_device_init(&irdev->rdev, &incoherent_rdev_ops, 0, size);
	irdev->read = read;
	irdev->write = write;

	return &irdev->rdev;
}
