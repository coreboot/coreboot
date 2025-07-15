/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/spi.h>
#include <boot_device.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <device/mmio.h>
#include <fmap.h>
#include <spi_flash.h>
#include <types.h>
#include "psp_def.h"
#include "psp_smi_flash.h"

static bool is_valid_rw_byte_count(struct mbox_psp_cmd_spi_read_write *cmd_buf,
				   uint64_t num_bytes)
{
	const uint32_t cmd_buf_size = read32(&cmd_buf->header.size);
	const size_t payload_buffer_offset =
		offsetof(struct mbox_psp_cmd_spi_read_write, req) +
		offsetof(struct psp_spi_read_write_request, buffer);
	return num_bytes <= cmd_buf_size - payload_buffer_offset;
}

static const char *id_to_region_name(uint64_t target_nv_id)
{
	switch (target_nv_id) {
	case SMI_TARGET_NVRAM:
		return "PSP_NVRAM";
	case SMI_TARGET_RPMC_NVRAM:
		return "PSP_RPMC_NVRAM";
	}
	return NULL;
}

/*
 * Do not cache the location to cope with flash changing underneath (e.g. due
 * to an update)
 */
static int lookup_store(uint64_t target_nv_id, struct region_device *rstore)
{
	/* read_rdev, write_rdev and store_irdev need to be static to not go out of scope when
	   this function returns */
	static struct region_device read_rdev, write_rdev;
	static struct incoherent_rdev store_irdev;
	const char *name;
	struct region region;
	const struct region_device *rdev;

	name = id_to_region_name(target_nv_id);
	if (!name)
		return -1;

	if (fmap_locate_area(name, &region) < 0)
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

static enum mbox_p2c_status get_flash_device(const struct spi_flash **flash)
{
	*flash = boot_device_spi_flash();
	if (*flash == NULL) {
		printk(BIOS_ERR, "PSP: Unable to find SPI device\n");
		return MBOX_PSP_COMMAND_PROCESS_ERROR;
	}

	return MBOX_PSP_SUCCESS;
}

static enum mbox_p2c_status find_psp_spi_flash_device_region(uint64_t target_nv_id,
							     struct region_device *store,
							     const struct spi_flash **flash)
{
	if (get_flash_device(flash) != MBOX_PSP_SUCCESS)
		return MBOX_PSP_COMMAND_PROCESS_ERROR;

	if (lookup_store(target_nv_id, store) < 0) {
		printk(BIOS_ERR, "PSP: Unable to find PSP SPI region\n");
		return MBOX_PSP_COMMAND_PROCESS_ERROR;
	}

	return MBOX_PSP_SUCCESS;
}

static bool spi_controller_busy(void)
{
	bool busy;

	/* When the firmware is using the SPI controller stop here */
	busy = (spi_read8(SPI_MISC_CNTRL) & SPI_SEMAPHORE_BIOS_LOCKED);
	if (busy) {
		printk(BIOS_NOTICE, "PSP: SPI controller blocked by coreboot (ring 0)\n");
		return true;
	}

	/*
	 * When ring0 is operating on the SPI flash and the controller is
	 * busy, don't interrupt ongoing transfer.
	 */
	if (spi_read32(SPI_STATUS) & SPI_BUSY)
		busy = true;

	/*
	 * Even when the SPI controller is not busy, the SPI flash
	 * might be busy. When that's the case reading from the
	 * memory mapped SPI flash doesn't work and returns all 0xffs.
	 * Thus check if the SPI flash is busy.
	 */
	if (CONFIG(SPI_FLASH) && !busy) {
		const struct spi_flash *spi_flash_dev;
		uint8_t sr1 = 0;

		spi_flash_dev = boot_device_spi_flash();
		assert(spi_flash_dev);
		if (spi_flash_dev) {
			/* Read Status Register 1 */
			if (spi_flash_status(spi_flash_dev, &sr1) < 0)
				busy = true;
			else if (sr1 & BIT(0))
				busy = true;
		}
	}

	if (CONFIG(SOC_AMD_PICASSO) && !busy) {
		// Only implemented on Picasso and Raven Ridge
		busy = (spi_read8(SPI_MISC_CNTRL) & SPI_SEMAPHORE_DRIVER_LOCKED);
	}

	if (busy)
		printk(BIOS_NOTICE, "PSP: SPI controller or SPI flash busy\n");

	return busy;
}

enum mbox_p2c_status psp_smi_spi_get_info(struct mbox_default_buffer *buffer)
{
	struct mbox_psp_cmd_spi_info *const cmd_buf =
		(struct mbox_psp_cmd_spi_info *)buffer;
	const struct spi_flash *flash;
	struct region_device store;
	uint64_t target_nv_id;
	uint64_t block_size;
	uint64_t num_blocks;
	enum mbox_p2c_status ret;

	printk(BIOS_SPEW, "PSP: SPI info request\n");

	if (!is_valid_psp_spi_info(cmd_buf))
		return MBOX_PSP_COMMAND_PROCESS_ERROR;

	if (spi_controller_busy()) {
		return MBOX_PSP_SPI_BUSY;
	}

	target_nv_id = get_psp_spi_info_id(cmd_buf);

	ret = find_psp_spi_flash_device_region(target_nv_id, &store, &flash);

	if (ret != MBOX_PSP_SUCCESS)
		return ret;

	block_size = flash->sector_size;

	if (!block_size)
		return MBOX_PSP_COMMAND_PROCESS_ERROR;

	num_blocks = region_device_sz(&store) / block_size;

	set_psp_spi_info(cmd_buf, 0, block_size, num_blocks);

	return MBOX_PSP_SUCCESS;
}

enum mbox_p2c_status psp_smi_spi_read(struct mbox_default_buffer *buffer)
{
	struct mbox_psp_cmd_spi_read_write *const cmd_buf =
		(struct mbox_psp_cmd_spi_read_write *)buffer;
	enum mbox_p2c_status ret;
	uint64_t target_nv_id;
	uint64_t lba;
	uint64_t offset;
	uint64_t num_bytes;
	uint8_t *data;
	size_t addr;
	const struct spi_flash *flash;
	struct region_device store;

	printk(BIOS_SPEW, "PSP: SPI read request\n");

	if (!is_valid_psp_spi_read_write(cmd_buf))
		return MBOX_PSP_COMMAND_PROCESS_ERROR;

	if (spi_controller_busy()) {
		return MBOX_PSP_SPI_BUSY;
	}

	get_psp_spi_read_write(cmd_buf, &target_nv_id, &lba, &offset, &num_bytes, &data);

	if (!is_valid_rw_byte_count(cmd_buf, num_bytes)) {
		printk(BIOS_ERR, "PSP: Read command requested more bytes than we have space "
				 "for in the buffer\n");
		return MBOX_PSP_COMMAND_PROCESS_ERROR;
	}

	ret = find_psp_spi_flash_device_region(target_nv_id, &store, &flash);

	if (ret != MBOX_PSP_SUCCESS)
		return ret;

	addr = (lba * flash->sector_size) + offset;

	printk(BIOS_SPEW, "PSP: SPI read 0x%llx bytes at 0x%zx\n", num_bytes, addr);

	if (rdev_readat(&store, data, addr, (size_t)num_bytes) != (size_t)num_bytes) {
		printk(BIOS_ERR, "PSP: Failed to read NVRAM data\n");
		return MBOX_PSP_COMMAND_PROCESS_ERROR;
	}

	return MBOX_PSP_SUCCESS;
}

enum mbox_p2c_status psp_smi_spi_write(struct mbox_default_buffer *buffer)
{
	struct mbox_psp_cmd_spi_read_write *const cmd_buf =
		(struct mbox_psp_cmd_spi_read_write *)buffer;
	enum mbox_p2c_status ret;
	uint64_t target_nv_id;
	uint64_t lba;
	uint64_t offset;
	uint64_t num_bytes;
	uint8_t *data;
	size_t addr;
	const struct spi_flash *flash;
	struct region_device store;

	printk(BIOS_SPEW, "PSP: SPI write request\n");

	if (!is_valid_psp_spi_read_write(cmd_buf))
		return MBOX_PSP_COMMAND_PROCESS_ERROR;

	if (spi_controller_busy()) {
		return MBOX_PSP_SPI_BUSY;
	}

	get_psp_spi_read_write(cmd_buf, &target_nv_id, &lba, &offset, &num_bytes, &data);

	if (!is_valid_rw_byte_count(cmd_buf, num_bytes)) {
		printk(BIOS_ERR, "PSP: Write command contains more bytes than we have space "
				 "for in the buffer\n");
		return MBOX_PSP_COMMAND_PROCESS_ERROR;
	}

	ret = find_psp_spi_flash_device_region(target_nv_id, &store, &flash);

	if (ret != MBOX_PSP_SUCCESS)
		return ret;

	addr = (lba * flash->sector_size) + offset;

	printk(BIOS_SPEW, "PSP: SPI write 0x%llx bytes at 0x%zx\n", num_bytes, addr);

	if (rdev_writeat(&store, data, addr, (size_t)num_bytes) != (size_t)num_bytes) {
		printk(BIOS_ERR, "PSP: Failed to write NVRAM data\n");
		return MBOX_PSP_COMMAND_PROCESS_ERROR;
	}

	return MBOX_PSP_SUCCESS;
}

enum mbox_p2c_status psp_smi_spi_erase(struct mbox_default_buffer *buffer)
{
	struct mbox_psp_cmd_spi_erase *const cmd_buf =
		(struct mbox_psp_cmd_spi_erase *)buffer;
	enum mbox_p2c_status ret;
	uint64_t target_nv_id;
	uint64_t lba;
	uint64_t num_blocks;
	size_t addr;
	size_t num_bytes;
	const struct spi_flash *flash;
	struct region_device store;

	printk(BIOS_SPEW, "PSP: SPI erase request\n");

	if (!is_valid_psp_spi_erase(cmd_buf))
		return MBOX_PSP_COMMAND_PROCESS_ERROR;

	if (spi_controller_busy()) {
		return MBOX_PSP_SPI_BUSY;
	}

	get_psp_spi_erase(cmd_buf, &target_nv_id, &lba, &num_blocks);

	ret = find_psp_spi_flash_device_region(target_nv_id, &store, &flash);

	if (ret != MBOX_PSP_SUCCESS)
		return ret;

	addr = lba * flash->sector_size;
	num_bytes = (size_t)num_blocks * flash->sector_size;

	printk(BIOS_SPEW, "PSP: SPI erase 0x%zx bytes at 0x%zx\n", num_bytes, addr);

	if (rdev_eraseat(&store, addr, num_bytes) != num_bytes) {
		printk(BIOS_ERR, "PSP: Failed to erase SPI NVRAM data\n");
		return MBOX_PSP_COMMAND_PROCESS_ERROR;
	}

	return MBOX_PSP_SUCCESS;
}

enum mbox_p2c_status psp_smi_spi_rpmc_inc_mc(struct mbox_default_buffer *buffer)
{
	struct mbox_psp_cmd_spi_rpmc_inc_mc *const cmd_buf =
		(struct mbox_psp_cmd_spi_rpmc_inc_mc *)buffer;
	const struct spi_flash *flash;

	printk(BIOS_SPEW, "PSP: SPI RPMC increment monotonic counter request\n");

	if (!CONFIG(SOC_AMD_COMMON_BLOCK_PSP_RPMC))
		return MBOX_PSP_UNSUPPORTED;

	if (spi_controller_busy()) {
		return MBOX_PSP_SPI_BUSY;
	}

	if (get_flash_device(&flash) != MBOX_PSP_SUCCESS)
		return MBOX_PSP_COMMAND_PROCESS_ERROR;

	if (spi_flash_rpmc_increment(flash, cmd_buf->req.counter_address,
				     cmd_buf->req.counter_data, cmd_buf->req.signature)
			!= CB_SUCCESS)
		return MBOX_PSP_COMMAND_PROCESS_ERROR;

	return MBOX_PSP_SUCCESS;
}

enum mbox_p2c_status psp_smi_spi_rpmc_req_mc(struct mbox_default_buffer *buffer)
{
	struct mbox_psp_cmd_spi_rpmc_req_mc *const cmd_buf =
		(struct mbox_psp_cmd_spi_rpmc_req_mc *)buffer;
	const struct spi_flash *flash;

	printk(BIOS_SPEW, "PSP: SPI RPMC request monotonic counter request\n");

	if (!CONFIG(SOC_AMD_COMMON_BLOCK_PSP_RPMC))
		return MBOX_PSP_UNSUPPORTED;

	if (spi_controller_busy()) {
		return MBOX_PSP_SPI_BUSY;
	}

	if (get_flash_device(&flash) != MBOX_PSP_SUCCESS)
		return MBOX_PSP_COMMAND_PROCESS_ERROR;

	if (spi_flash_rpmc_request(flash, cmd_buf->req.counter_address, cmd_buf->req.tag,
				   cmd_buf->req.signature, cmd_buf->req.output_counter_data,
				   cmd_buf->req.output_signature) != CB_SUCCESS)
		return MBOX_PSP_COMMAND_PROCESS_ERROR;

	return MBOX_PSP_SUCCESS;
}
