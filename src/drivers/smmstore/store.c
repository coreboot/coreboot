/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>
#include <commonlib/helpers.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <fmap.h>
#include <fmap_config.h>
#include <smmstore.h>
#include <types.h>

#define SMMSTORE_REGION "SMMSTORE"


_Static_assert(IS_ALIGNED(FMAP_SECTION_SMMSTORE_START, SMM_BLOCK_SIZE),
	       "SMMSTORE FMAP region not aligned to 64K");

_Static_assert(SMM_BLOCK_SIZE <= FMAP_SECTION_SMMSTORE_SIZE,
	       "SMMSTORE FMAP region must be at least 64K");

static int smmstore_use_full_flash;
static int has_capsules = -1;

int smmstore_preprocess_cmd(uint8_t *cmd, void *param)
{
	if (CONFIG(DRIVERS_EFI_UPDATE_CAPSULES)) {
		if (has_capsules == -1 && *cmd == SMMSTORE_CMD_USE_FULL_FLASH) {
			has_capsules = !!(uintptr_t)param;
			/*
			 * If we have capsules, return success, otherwise let smmstore_exec()
			 * fail on !param check, which will be 0 in that case. This informs
			 * the caller whether capsule handling was enabled or not.
			 */
			return has_capsules;
		} else if (has_capsules == 1 && *cmd & SMMSTORE_CMD_USE_FULL_FLASH) {
			smmstore_use_full_flash = 1;
			*cmd &= ~SMMSTORE_CMD_USE_FULL_FLASH;
		} else {
			smmstore_use_full_flash = 0;
		}
	}

	return 0;
}

static enum cb_err lookup_store_region(struct region *region)
{
	if (CONFIG(DRIVERS_EFI_UPDATE_CAPSULES) && smmstore_use_full_flash) {
		const struct region_device *rdev = boot_device_rw();

		if (rdev == NULL)
			return CB_ERR;

		*region = *region_device_region(rdev);
		return CB_SUCCESS;
	}

	if (fmap_locate_area(SMMSTORE_REGION, region)) {
		printk(BIOS_WARNING,
		       "smm store: Unable to find SMM store FMAP region '%s'\n",
		       SMMSTORE_REGION);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

/*
 * Return a region device that points into the store file.
 *
 * It's the image builder's responsibility to make it block aligned so that
 * erase works without destroying other data.
 *
 * It doesn't cache the location to cope with flash changing underneath (eg
 * due to an update)
 *
 * returns 0 on success, -1 on failure
 * outputs the valid store rdev in rstore
 */
static int lookup_store(struct region_device *rstore)
{
	static struct region_device read_rdev, write_rdev;
	static struct incoherent_rdev store_irdev;
	struct region region;
	const struct region_device *rdev;

	if (lookup_store_region(&region) != CB_SUCCESS)
		return -1;

	if (boot_device_ro_subregion(&region, &read_rdev) < 0)
		return -1;

	if (boot_device_rw_subregion(&region, &write_rdev) < 0)
		return -1;

	rdev = incoherent_rdev_init(&store_irdev, &region, &read_rdev, &write_rdev);

	if (rdev == NULL)
		return -1;

	return rdev_chain(rstore, rdev, 0, region_device_sz(rdev));
}

 /* this function is non reentrant */
int smmstore_lookup_region(struct region_device *rstore)
{
	static int done;
	static int ret;
	static struct region_device rdev;

	if (!done) {
		done = 1;

		if (lookup_store(&rdev)) {
			printk(BIOS_WARNING,
			       "smm store: Unable to find SMM store FMAP region '%s'\n",
				SMMSTORE_REGION);
			ret = -1;
		} else {
			ret = 0;
		}
	}

	*rstore = rdev;
	return ret;
}
static struct region_device mdev_com_buf;

static int smmstore_rdev_chain(struct region_device *rdev)
{
	return rdev_chain_full(rdev, &mdev_com_buf);
}

/**
 * Call once before using the store. In SMM this must be called through an
 * APM SMI handler providing the communication buffer address and length.
 */
int smmstore_init(void *buf, size_t len)
{
	if (!buf || len < SMM_BLOCK_SIZE)
		return -1;

	if (smm_points_to_smram(buf, len))
		return -1;

	rdev_chain_mem_rw(&mdev_com_buf, buf, len);

	return 0;
}

#if ENV_RAMSTAGE
/**
 * Provide metadata for the coreboot tables.
 * Must only be called in ramstage, but not in SMM.
 */
int smmstore_get_info(struct smmstore_params_info *out)
{
	struct region_device store;

	if (lookup_store(&store) < 0) {
		printk(BIOS_ERR, "smm store: lookup of store failed\n");
		return -1;
	}

	if (!IS_ALIGNED(region_device_offset(&store), SMM_BLOCK_SIZE)) {
		printk(BIOS_ERR, "smm store: store not aligned to block size\n");
		return -1;
	}

	out->block_size = SMM_BLOCK_SIZE;
	out->num_blocks = region_device_sz(&store) / SMM_BLOCK_SIZE;

	/* FIXME: Broken edk2 always assumes memory mapped Firmware Block Volumes */
	out->mmap_addr = (uintptr_t)rdev_mmap_full(&store);

	printk(BIOS_DEBUG, "smm store: %d # blocks with size 0x%x\n",
	       out->num_blocks, out->block_size);

	return 0;
}
#endif

/* Returns -1 on error, 0 on success */
static int lookup_block_in_store(struct region_device *store, uint32_t block_id)
{
	if (lookup_store(store) < 0) {
		printk(BIOS_ERR, "smm store: lookup of store failed\n");
		return -1;
	}

	if ((block_id * SMM_BLOCK_SIZE) >= region_device_sz(store)) {
		printk(BIOS_ERR, "smm store: block ID out of range\n");
		return -1;
	}

	return 0;
}

/* Returns NULL on error, pointer from rdev_mmap on success */
static void *mmap_com_buf(struct region_device *com_buf, uint32_t offset, uint32_t bufsize)
{
	if (smmstore_rdev_chain(com_buf) < 0) {
		printk(BIOS_ERR, "smm store: lookup of com buffer failed\n");
		return NULL;
	}

	if (offset >= region_device_sz(com_buf)) {
		printk(BIOS_ERR, "smm store: offset out of range\n");
		return NULL;
	}

	void *ptr = rdev_mmap(com_buf, offset, bufsize);
	if (!ptr)
		printk(BIOS_ERR, "smm store: not enough space for new data\n");

	return ptr;
}

/**
 * Reads the specified block of the SMMSTORE and places it in the communication
 * buffer.
 * @param block_id The id of the block to operate on
 * @param offset Offset within the block.
 *               Must be smaller than the block size.
 * @param bufsize Size of chunk to read within the block.
 *                Must be smaller than the block size.

 * @return Returns -1 on error, 0 on success.
 */
int smmstore_rawread_region(uint32_t block_id, uint32_t offset, uint32_t bufsize)
{
	struct region_device store;
	struct region_device com_buf;

	if (lookup_block_in_store(&store, block_id) < 0)
		return -1;

	void *ptr = mmap_com_buf(&com_buf, offset, bufsize);
	if (!ptr)
		return -1;

	printk(BIOS_DEBUG, "smm store: reading %p block %d, offset=0x%x, size=%x\n",
	       ptr, block_id, offset, bufsize);

	ssize_t ret = rdev_readat(&store, ptr, block_id * SMM_BLOCK_SIZE + offset, bufsize);
	rdev_munmap(&com_buf, ptr);
	if (ret < 0)
		return -1;

	return 0;
}

/**
 * Writes the specified block of the SMMSTORE by reading it from the communication
 * buffer.
 * @param block_id The id of the block to operate on
 * @param offset Offset within the block.
 *               Must be smaller than the block size.
 * @param bufsize Size of chunk to read within the block.
 *                Must be smaller than the block size.

 * @return Returns -1 on error, 0 on success.
 */
int smmstore_rawwrite_region(uint32_t block_id, uint32_t offset, uint32_t bufsize)
{
	struct region_device store;
	struct region_device com_buf;

	if (lookup_block_in_store(&store, block_id) < 0)
		return -1;

	if (rdev_chain(&store, &store, block_id * SMM_BLOCK_SIZE + offset, bufsize)) {
		printk(BIOS_ERR, "smm store: not enough space for new data\n");
		return -1;
	}

	void *ptr = mmap_com_buf(&com_buf, offset, bufsize);
	if (!ptr)
		return -1;

	printk(BIOS_DEBUG, "smm store: writing %p block %d, offset=0x%x, size=%x\n",
	       ptr, block_id, offset, bufsize);

	ssize_t ret = rdev_writeat(&store, ptr, 0, bufsize);
	rdev_munmap(&com_buf, ptr);
	if (ret < 0)
		return -1;

	return 0;
}

/**
 * Erases the specified block of the SMMSTORE. The communication buffer remains untouched.
 *
 * @param block_id The id of the block to operate on
 *
 * @return Returns -1 on error, 0 on success.
 */
int smmstore_rawclear_region(uint32_t block_id)
{
	struct region_device store;

	if (lookup_block_in_store(&store, block_id) < 0)
		return -1;

	ssize_t ret = rdev_eraseat(&store, block_id * SMM_BLOCK_SIZE, SMM_BLOCK_SIZE);
	if (ret != SMM_BLOCK_SIZE) {
		printk(BIOS_ERR, "smm store: erasing block failed\n");
		return -1;
	}

	return 0;
}
