/* SPDX-License-Identifier: GPL-2.0-only */

#include "psp_verstage.h"

#include <bl_uapp/bl_errorcodes_public.h>
#include <bl_uapp/bl_syscall_public.h>
#include <boot_device.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <string.h>

#define DEST_BUF_ALIGNMENT 16

static uint32_t active_map_count;

static void *boot_dev_mmap(const struct region_device *rd, size_t offset, size_t size)
{
	const struct mem_region_device *mdev;
	void *mapping = NULL;
	uint32_t ret;

	mdev = container_of(rd, __typeof__(*mdev), rdev);

	if (mdev->base) {
		if ((ret = svc_map_spi_rom(&mdev->base[offset], size, (void **)&mapping))
										!= BL_OK)
			printk(BIOS_ERR, "Failed(%d) to map SPI ROM offset: %zu, size: %zu\n",
									ret, offset, size);
		else
			active_map_count++;
	}

	return mapping;
}

static int boot_dev_munmap(const struct region_device *rd, void *mapping)
{
	uint32_t ret;

	active_map_count--;
	if ((ret = svc_unmap_spi_rom(mapping)) != BL_OK)
		printk(BIOS_ERR, "Failed(%d) to unmap SPI ROM mapping %p\n", ret, mapping);
	return 0;
}

static ssize_t boot_dev_mmap_readat(const struct region_device *rd, void *dest,
							size_t offset, size_t size)
{
	void *mapping = boot_dev_mmap(rd, offset, size);

	if (!mapping)
		return -1;

	memcpy(dest, mapping, size);
	boot_dev_munmap(rd, mapping);
	return size;
}

static ssize_t boot_dev_dma_readat(const struct region_device *rd, void *dest,
							size_t offset, size_t size)
{
	size_t memcpy_size = ALIGN_UP((uintptr_t)dest, DEST_BUF_ALIGNMENT) - (uintptr_t)dest;
	const struct mem_region_device *mdev = container_of(rd, __typeof__(*mdev), rdev);
	int ret;

	if (memcpy_size > size)
		memcpy_size = size;
	/* Alignment requirement is only on dest buffer for CCP DMA. So do a memcpy
	   until the destination buffer alignment requirement is met. */
	if (memcpy_size && boot_dev_mmap_readat(rd, dest, offset, memcpy_size) != memcpy_size)
		return -1;

	dest = ((char *)dest + memcpy_size);
	offset += memcpy_size;
	size -= memcpy_size;
	if (!size)
		return memcpy_size;

	ret = svc_ccp_dma((uint32_t)offset, dest, (uint32_t)size);
	if (ret != BL_OK) {
		printk(BIOS_ERR, "%s: Failed dest:%p offset:%zu size:%zu ret:%d\n",
						__func__, dest, offset, size, ret);
		return -1;
	}

	return size + memcpy_size;
}

static ssize_t boot_dev_readat(const struct region_device *rd, void *dest,
						size_t offset, size_t size)
{
	const struct mem_region_device *mdev = container_of(rd, __typeof__(*mdev), rdev);

	if (CONFIG(PSP_VERSTAGE_CCP_DMA))
		return boot_dev_dma_readat(rd, dest, offset, size);

	return boot_dev_mmap_readat(rd, dest, offset, size);
}

const struct region_device_ops boot_dev_rdev_ro_ops = {
	.mmap = boot_dev_mmap,
	.munmap = boot_dev_munmap,
	.readat = boot_dev_readat,
};

static struct mem_region_device boot_dev = {
	.rdev = REGION_DEV_INIT(&boot_dev_rdev_ro_ops, 0, CONFIG_ROM_SIZE),
};

const struct region_device *boot_device_ro(void)
{
	if (!boot_dev.base)
		boot_dev.base = (void *)map_spi_rom();
	return &boot_dev.rdev;
}

uint32_t boot_dev_get_active_map_count(void)
{
	return active_map_count;
}
