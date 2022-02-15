/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cbfs.h>
#include <console/console.h>
#include <delay.h>
#include <ec/google/chromeec/ec.h>
#include <halt.h>
#include <security/vboot/misc.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/vboot_common.h>
#include <timer.h>
#include <timestamp.h>
#include <vb2_api.h>

#define _EC_FILENAME(select, suffix) \
	(select == VB_SELECT_FIRMWARE_READONLY ? "ecro" suffix : "ecrw" suffix)
#define EC_IMAGE_FILENAME(select) _EC_FILENAME(select, "")
#define EC_HASH_FILENAME(select) _EC_FILENAME(select, ".hash")

/* Wait 10 ms between attempts to check if EC's hash is ready */
#define CROS_EC_HASH_CHECK_DELAY_MS 10
/* Give the EC 2 seconds to finish calculating its hash */
#define CROS_EC_HASH_TIMEOUT_MS 2000

/* Wait 3 seconds after software sync for EC to clear the limit power flag. */
#define LIMIT_POWER_WAIT_TIMEOUT_MS 3000
/* Check the limit power flag every 10 ms while waiting. */
#define LIMIT_POWER_POLL_SLEEP_MS 10

/* Wait 3 seconds for EC to sysjump to RW */
#define CROS_EC_SYSJUMP_TIMEOUT_MS 3000

/*
 * The external API for EC software sync.  This function calls into
 * vboot, which kicks off the process.  Vboot runs the verified boot
 * logic, and requires the client program to provide callbacks which
 * perform the work.
 */
void vboot_sync_ec(void)
{
	vb2_error_t retval = VB2_SUCCESS;
	struct vb2_context *ctx;

	timestamp_add_now(TS_EC_SYNC_START);

	ctx = vboot_get_context();
	ctx->flags |= VB2_CONTEXT_EC_SYNC_SUPPORTED;

	retval = vb2api_ec_sync(ctx);
	vboot_save_data(ctx);

	switch (retval) {
	case VB2_SUCCESS:
		break;

	case VB2_REQUEST_REBOOT_EC_TO_RO:
		printk(BIOS_INFO, "EC Reboot requested. Doing cold reboot\n");
		if (google_chromeec_reboot(0, EC_REBOOT_COLD, 0))
			printk(BIOS_EMERG, "Failed to get EC to cold reboot\n");

		halt();
		break;

	/* Only for EC-EFS */
	case VB2_REQUEST_REBOOT_EC_SWITCH_RW:
		printk(BIOS_INFO, "Switch EC slot requested. Doing cold reboot\n");
		if (google_chromeec_reboot(0, EC_REBOOT_COLD,
						EC_REBOOT_FLAG_SWITCH_RW_SLOT))
			printk(BIOS_EMERG, "Failed to get EC to cold reboot\n");

		halt();
		break;

	case VB2_REQUEST_REBOOT:
		printk(BIOS_INFO, "Reboot requested. Doing warm reboot\n");
		vboot_reboot();
		break;

	default:
		printk(BIOS_ERR, "EC software sync failed (%#x),"
			" rebooting\n", retval);
		vboot_reboot();
		break;
	}

	timestamp_add_now(TS_EC_SYNC_END);
}

/* Convert firmware image type into a flash offset */
static uint32_t get_vboot_hash_offset(enum vb2_firmware_selection select)
{
	switch (select) {
	case VB_SELECT_FIRMWARE_READONLY:
		return EC_VBOOT_HASH_OFFSET_RO;
	case VB_SELECT_FIRMWARE_EC_UPDATE:
		return EC_VBOOT_HASH_OFFSET_UPDATE;
	default:
		return EC_VBOOT_HASH_OFFSET_ACTIVE;
	}
}

/*
 * Asks the EC to calculate a hash of the specified firmware image, and
 * returns the information in **hash and *hash_size.
 */
static vb2_error_t ec_hash_image(enum vb2_firmware_selection select,
				 const uint8_t **hash, int *hash_size)
{
	static struct ec_response_vboot_hash resp;
	uint32_t hash_offset;
	int recalc_requested = 0;
	struct stopwatch sw;

	hash_offset = get_vboot_hash_offset(select);

	stopwatch_init_msecs_expire(&sw, CROS_EC_HASH_TIMEOUT_MS);
	do {
		if (google_chromeec_get_vboot_hash(hash_offset, &resp))
			return VB2_ERROR_UNKNOWN;

		switch (resp.status) {
		case EC_VBOOT_HASH_STATUS_NONE:
			/*
			 * There is no hash available right now.
			 * Request a recalc if it hasn't been done yet.
			 */
			if (recalc_requested)
				break;

			printk(BIOS_WARNING,
			       "%s: No valid hash (status=%d size=%d). "
			       "Computing...\n", __func__, resp.status,
			       resp.size);

			if (google_chromeec_start_vboot_hash(
				    EC_VBOOT_HASH_TYPE_SHA256, hash_offset, &resp))
				return VB2_ERROR_UNKNOWN;

			recalc_requested = 1;

			/*
			 * Expect status to be busy since we just sent
			 * a recalc request.
			 */
			resp.status = EC_VBOOT_HASH_STATUS_BUSY;

			/* Hash just started calculating, let it go for a bit */
			mdelay(CROS_EC_HASH_CHECK_DELAY_MS);
			break;

		case EC_VBOOT_HASH_STATUS_BUSY:
			/* Hash is still calculating. */
			mdelay(CROS_EC_HASH_CHECK_DELAY_MS);
			break;

		case EC_VBOOT_HASH_STATUS_DONE: /* intentional fallthrough */
		default:
			/* Hash is ready! */
			break;
		}
	} while (resp.status == EC_VBOOT_HASH_STATUS_BUSY &&
		 !stopwatch_expired(&sw));

	timestamp_add_now(TS_EC_HASH_READY);

	if (resp.status != EC_VBOOT_HASH_STATUS_DONE) {
		printk(BIOS_ERR, "%s: Hash status not done: %d\n", __func__,
		       resp.status);
		return VB2_ERROR_UNKNOWN;
	}
	if (resp.hash_type != EC_VBOOT_HASH_TYPE_SHA256) {
		printk(BIOS_ERR, "EC hash was the wrong type.\n");
		return VB2_ERROR_UNKNOWN;
	}

	printk(BIOS_INFO, "EC took %luus to calculate image hash\n",
		stopwatch_duration_usecs(&sw));

	*hash = resp.hash_digest;
	*hash_size = resp.digest_size;

	return VB2_SUCCESS;
}

/*
 * Asks the EC to protect or unprotect the specified flash region.
 */
static vb2_error_t ec_protect_flash(enum vb2_firmware_selection select, int enable)
{
	struct ec_response_flash_protect resp;
	uint32_t protected_region = EC_FLASH_PROTECT_ALL_NOW;
	const uint32_t mask = EC_FLASH_PROTECT_ALL_NOW | EC_FLASH_PROTECT_ALL_AT_BOOT;

	if (select == VB_SELECT_FIRMWARE_READONLY)
		protected_region = EC_FLASH_PROTECT_RO_NOW;

	if (google_chromeec_flash_protect(mask, enable ? mask : 0, &resp) != 0)
		return VB2_ERROR_UNKNOWN;

	if (!enable) {
		/* If protection is still enabled, need reboot */
		if (resp.flags & protected_region)
			return VB2_REQUEST_REBOOT_EC_TO_RO;

		return VB2_SUCCESS;
	}

	/*
	 * If write protect and ro-at-boot aren't both asserted, don't expect
	 * protection enabled.
	 */
	if ((~resp.flags) & (EC_FLASH_PROTECT_GPIO_ASSERTED |
			     EC_FLASH_PROTECT_RO_AT_BOOT))
		return VB2_SUCCESS;

	/* If flash is protected now, success */
	if (resp.flags & EC_FLASH_PROTECT_ALL_NOW)
		return VB2_SUCCESS;

	/* If RW will be protected at boot but not now, need a reboot */
	if (resp.flags & EC_FLASH_PROTECT_ALL_AT_BOOT)
		return VB2_REQUEST_REBOOT_EC_TO_RO;

	/* Otherwise, it's an error */
	return VB2_ERROR_UNKNOWN;
}

/* Convert a firmware image type to an EC flash region */
static enum ec_flash_region vboot_to_ec_region(enum vb2_firmware_selection select)
{
	switch (select) {
	case VB_SELECT_FIRMWARE_READONLY:
		return EC_FLASH_REGION_WP_RO;
	case VB_SELECT_FIRMWARE_EC_UPDATE:
		return EC_FLASH_REGION_UPDATE;
	default:
		return EC_FLASH_REGION_ACTIVE;
	}
}

/*
 * Send an image to the EC in burst-sized chunks.
 */
static vb2_error_t ec_flash_write(void *image, uint32_t region_offset,
				  int image_size)
{
	struct ec_response_get_protocol_info resp_proto;
	struct ec_response_flash_info resp_flash;
	ssize_t pdata_max_size;
	ssize_t burst;
	uint8_t *file_buf;
	struct ec_params_flash_write *params;
	uint32_t end, off;

	/*
	 * Get EC's protocol information, so that we can figure out how much
	 * data can be sent in one message.
	 */
	if (google_chromeec_get_protocol_info(&resp_proto)) {
		printk(BIOS_ERR, "Failed to get EC protocol information; "
		       "skipping flash write\n");
		return VB2_ERROR_UNKNOWN;
	}

	/*
	 * Determine burst size.  This must be a multiple of the write block
	 * size, and must also fit into the host parameter buffer.
	 */
	if (google_chromeec_flash_info(&resp_flash)) {
		printk(BIOS_ERR, "Failed to get EC flash information; "
		       "skipping flash write\n");
		return VB2_ERROR_UNKNOWN;
	}

	/* Limit the potential buffer stack allocation to 1K */
	pdata_max_size = MIN(1024, resp_proto.max_request_packet_size -
				   sizeof(struct ec_host_request));

	/* Round burst to a multiple of the flash write block size */
	burst = pdata_max_size - sizeof(*params);
	burst = (burst / resp_flash.write_block_size) *
		resp_flash.write_block_size;

	/* Buffer too small */
	if (burst <= 0) {
		printk(BIOS_ERR, "Flash write buffer too small!  skipping "
		       "flash write\n");
		return VB2_ERROR_UNKNOWN;
	}

	/* Allocate buffer on the stack */
	params = alloca(burst + sizeof(*params));

	/* Fill up the buffer */
	end = region_offset + image_size;
	file_buf = image;
	for (off = region_offset; off < end; off += burst) {
		uint32_t todo = MIN(end - off, burst);
		uint32_t xfer_size = todo + sizeof(*params);

		params->offset = off;
		params->size = todo;

		/* Read todo bytes into the buffer */
		memcpy(params + 1, file_buf, todo);

		/* Make sure to add back in the size of the parameters */
		if (google_chromeec_flash_write_block(
				(const uint8_t *)params, xfer_size)) {
			printk(BIOS_ERR, "EC failed flash write command, "
				"relative offset %u!\n", off - region_offset);
			return VB2_ERROR_UNKNOWN;
		}

		file_buf += todo;
	}

	return VB2_SUCCESS;
}

/*
 * The logic for updating an EC firmware image.
 */
static vb2_error_t ec_update_image(enum vb2_firmware_selection select)
{
	uint32_t region_offset, region_size;
	enum ec_flash_region region;
	vb2_error_t rv;
	void *image;
	size_t image_size;

	/* Un-protect the flash region */
	rv = ec_protect_flash(select, 0);
	if (rv != VB2_SUCCESS)
		return rv;

	/* Convert vboot region into an EC region */
	region = vboot_to_ec_region(select);

	/* Get information about the flash region */
	if (google_chromeec_flash_region_info(region, &region_offset,
					      &region_size))
		return VB2_ERROR_UNKNOWN;

	/* Map the CBFS file */
	image = cbfs_map(EC_IMAGE_FILENAME(select), &image_size);
	if (!image)
		return VB2_ERROR_UNKNOWN;

	rv = VB2_ERROR_UNKNOWN;

	/* Bail if the image is too large */
	if (image_size > region_size)
		goto unmap;

	/* Erase the region */
	if (google_chromeec_flash_erase(region_offset, region_size))
		goto unmap;

	/* Write the image into the region */
	if (ec_flash_write(image, region_offset, image_size))
		goto unmap;

	/* Verify the image */
	if (google_chromeec_efs_verify(region))
		goto unmap;

	rv = VB2_SUCCESS;

unmap:
	cbfs_unmap(image);
	return rv;
}

static vb2_error_t ec_get_expected_hash(enum vb2_firmware_selection select,
					const uint8_t **hash,
					int *hash_size)
{
	size_t size;
	const char *filename = EC_HASH_FILENAME(select);

	/* vboot has no API to return this memory, so must permanently leak a mapping here. */
	const uint8_t *file = cbfs_map(filename, &size);

	if (file == NULL)
		return VB2_ERROR_UNKNOWN;

	*hash = file;
	*hash_size = (int)size;

	return VB2_SUCCESS;
}

/***********************************************************************
 * Vboot Callbacks
 ***********************************************************************/

/*
 * Write opaque data into NV storage region.
 */
vb2_error_t vb2ex_commit_data(struct vb2_context *ctx)
{
	save_vbnv(ctx->nvdata);
	return VB2_SUCCESS;
}

/*
 * Report whether the EC is in RW or not.
 */
vb2_error_t vb2ex_ec_running_rw(int *in_rw)
{
	*in_rw = !google_ec_running_ro();
	return VB2_SUCCESS;
}

/*
 * Callback for when Vboot is finished.
 */
vb2_error_t vb2ex_ec_vboot_done(struct vb2_context *ctx)
{
	int limit_power = 0;
	bool message_printed = false;
	struct stopwatch sw;
	int in_recovery = !!(ctx->flags & VB2_CONTEXT_RECOVERY_MODE);

	/*
	 * Do not wait for the limit power flag to be cleared in
	 * recovery mode since we didn't just sysjump.
	 */
	if (in_recovery)
		return VB2_SUCCESS;

	timestamp_add_now(TS_EC_POWER_LIMIT_WAIT);

	stopwatch_init_msecs_expire(&sw, LIMIT_POWER_WAIT_TIMEOUT_MS);

	/* Ensure we have enough power to continue booting. */
	while (1) {
		if (google_chromeec_read_limit_power_request(&limit_power)) {
			printk(BIOS_ERR, "Failed to check EC limit power"
			       "flag.\n");
			return VB2_ERROR_UNKNOWN;
		}

		if (!limit_power || stopwatch_expired(&sw))
			break;

		if (!message_printed) {
			printk(BIOS_SPEW,
			       "Waiting for EC to clear limit power flag.\n");
			message_printed = true;
		}

		mdelay(LIMIT_POWER_POLL_SLEEP_MS);
	}

	if (limit_power) {
		printk(BIOS_INFO,
		       "EC requests limited power usage. Request shutdown.\n");
		return VB2_REQUEST_SHUTDOWN;
	} else {
		printk(BIOS_INFO, "Waited %luus to clear limit power flag.\n",
			stopwatch_duration_usecs(&sw));
	}

	return VB2_SUCCESS;
}

/*
 * Support battery cutoff if required.
 */
vb2_error_t vb2ex_ec_battery_cutoff(void)
{
	if (google_chromeec_battery_cutoff(EC_BATTERY_CUTOFF_FLAG_AT_SHUTDOWN))
		return VB2_ERROR_UNKNOWN;

	return VB2_SUCCESS;
}

/*
 * Vboot callback for calculating an EC image's hash.
 */
vb2_error_t vb2ex_ec_hash_image(enum vb2_firmware_selection select,
				const uint8_t **hash, int *hash_size)
{
	return ec_hash_image(select, hash, hash_size);
}

/*
 * Vboot callback for EC flash protection.
 */
vb2_error_t vb2ex_ec_protect(enum vb2_firmware_selection select)
{
	return ec_protect_flash(select, 1);
}

/*
 * Get hash for image.
 */
vb2_error_t vb2ex_ec_get_expected_image_hash(enum vb2_firmware_selection select,
					     const uint8_t **hash,
					     int *hash_size)
{
	return ec_get_expected_hash(select, hash, hash_size);
}

/*
 * Disable further sysjumps (i.e., stay in RW until next reboot)
 */
vb2_error_t vb2ex_ec_disable_jump(void)
{
	if (google_chromeec_reboot(0, EC_REBOOT_DISABLE_JUMP, 0))
		return VB2_ERROR_UNKNOWN;

	return VB2_SUCCESS;
}

/*
 * Update EC image.
 */
vb2_error_t vb2ex_ec_update_image(enum vb2_firmware_selection select)
{
	return ec_update_image(select);
}

/*
 * Vboot callback for commanding EC to sysjump to RW.
 */
vb2_error_t vb2ex_ec_jump_to_rw(void)
{
	struct stopwatch sw;

	if (google_chromeec_reboot(0, EC_REBOOT_JUMP_RW, 0))
		return VB2_ERROR_UNKNOWN;

	/* Give the EC 3 seconds to sysjump */
	stopwatch_init_msecs_expire(&sw, CROS_EC_SYSJUMP_TIMEOUT_MS);

	/* Default delay to wait after EC reboot */
	mdelay(50);
	while (google_chromeec_hello()) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "EC did not return from reboot after %luus\n",
			       stopwatch_duration_usecs(&sw));
			return VB2_ERROR_UNKNOWN;
		}

		mdelay(5);
	}

	printk(BIOS_INFO, "\nEC returned from reboot after %luus\n",
	       stopwatch_duration_usecs(&sw));

	return VB2_SUCCESS;
}
