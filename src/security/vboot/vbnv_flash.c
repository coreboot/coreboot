/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/region.h>
#include <console/console.h>
#include <fmap.h>
#include <fmap_config.h>
#include <string.h>
#include <vb2_api.h>
#include <security/vboot/vboot_common.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/vbnv_layout.h>

#define BLOB_SIZE VB2_NVDATA_SIZE

struct vbnv_flash_ctx {
	/* VBNV flash is initialized */
	int initialized;

	/* Offset of the current nvdata in flash */
	int blob_offset;

	/* Offset of the topmost nvdata blob in flash */
	int top_offset;

	/* Region to store and retrieve the VBNV contents. */
	struct region_device vbnv_dev;

	/* Cache of the current nvdata */
	uint8_t cache[BLOB_SIZE];
};
static struct vbnv_flash_ctx vbnv_flash;

/*
 * This code assumes that flash is erased to 1-bits, and write operations can
 * only change 1-bits to 0-bits. So if the new contents only change 1-bits to
 * 0-bits, we can reuse the current blob.
 */
static inline uint8_t erase_value(void)
{
	return 0xff;
}

static inline int can_overwrite(uint8_t current, uint8_t new)
{
	return (current & new) == new;
}

_Static_assert(FMAP_SECTION_RW_NVRAM_SIZE >= BLOB_SIZE,
	       "RW_NVRAM FMAP section not present or too small");

static int init_vbnv(void)
{
	struct vbnv_flash_ctx *ctx = &vbnv_flash;
	struct region_device *rdev = &ctx->vbnv_dev;
	uint8_t buf[BLOB_SIZE];
	uint8_t empty_blob[BLOB_SIZE];
	int used_below, empty_above;
	int offset;
	int i;

	if (fmap_locate_area_as_rdev_rw("RW_NVRAM", rdev) ||
	    region_device_sz(rdev) < BLOB_SIZE) {
		printk(BIOS_ERR, "%s: failed to locate NVRAM\n", __func__);
		return 1;
	}

	/* Prepare an empty blob to compare against. */
	for (i = 0; i < BLOB_SIZE; i++)
		empty_blob[i] = erase_value();

	ctx->top_offset = region_device_sz(rdev) - BLOB_SIZE;

	/* Binary search for the border between used and empty */
	used_below = 0;
	empty_above = region_device_sz(rdev) / BLOB_SIZE;

	while (used_below + 1 < empty_above) {
		int guess = (used_below + empty_above) / 2;
		if (rdev_readat(rdev, buf, guess * BLOB_SIZE, BLOB_SIZE) < 0) {
			printk(BIOS_ERR, "failed to read nvdata\n");
			return 1;
		}
		if (!memcmp(buf, empty_blob, BLOB_SIZE))
			empty_above = guess;
		else
			used_below = guess;
	}

	/*
	 * Offset points to the last non-empty blob.  Or if all blobs are empty
	 * (nvram is totally erased), point to the first blob.
	 */
	offset = used_below * BLOB_SIZE;

	/* reread the nvdata and write it to the cache */
	if (rdev_readat(rdev, ctx->cache, offset, BLOB_SIZE) < 0) {
		printk(BIOS_ERR, "failed to read nvdata\n");
		return 1;
	}

	ctx->blob_offset = offset;
	ctx->initialized = 1;

	return 0;
}

static int erase_nvram(void)
{
	struct vbnv_flash_ctx *ctx = &vbnv_flash;
	const struct region_device *rdev = &ctx->vbnv_dev;

	if (rdev_eraseat(rdev, 0, region_device_sz(rdev)) < 0) {
		printk(BIOS_ERR, "failed to erase nvram\n");
		return 1;
	}

	printk(BIOS_INFO, "nvram is cleared\n");
	return 0;
}

void read_vbnv_flash(uint8_t *vbnv_copy)
{
	struct vbnv_flash_ctx *ctx = &vbnv_flash;

	if (!ctx->initialized)
		if (init_vbnv())
			return;  /* error */

	memcpy(vbnv_copy, ctx->cache, BLOB_SIZE);
}

void save_vbnv_flash(const uint8_t *vbnv_copy)
{
	struct vbnv_flash_ctx *ctx = &vbnv_flash;
	int new_offset;
	int i;
	const struct region_device *rdev = &ctx->vbnv_dev;

	if (!ctx->initialized)
		if (init_vbnv())
			return;  /* error */

	/* Bail out if there have been no changes. */
	if (!memcmp(vbnv_copy, ctx->cache, BLOB_SIZE))
		return;

	new_offset = ctx->blob_offset;

	/* See if we can overwrite the current blob with the new one */
	for (i = 0; i < BLOB_SIZE; i++) {
		if (!can_overwrite(ctx->cache[i], vbnv_copy[i])) {
			/* unable to overwrite. need to use the next blob */
			new_offset += BLOB_SIZE;
			if (new_offset > ctx->top_offset) {
				if (erase_nvram())
					return;  /* error */
				new_offset = 0;
			}
			break;
		}
	}

	if (rdev_writeat(rdev, vbnv_copy, new_offset, BLOB_SIZE) == BLOB_SIZE) {
		/* write was successful. safely move pointer forward */
		ctx->blob_offset = new_offset;
		memcpy(ctx->cache, vbnv_copy, BLOB_SIZE);
	} else {
		printk(BIOS_ERR, "failed to save nvdata\n");
	}
}
