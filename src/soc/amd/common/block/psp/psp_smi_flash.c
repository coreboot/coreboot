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

enum psp_spi_id_type {
	SMI_TARGET_NVRAM		= 0,
	SMI_TARGET_RPMC_NVRAM		= 5,
};

struct pspv2_spi_info_request {
	u64 target_nv_id;
	u64 lba;
	u64 block_size;
	u64 num_blocks;
} __packed;

struct mbox_pspv2_cmd_spi_info {
	struct mbox_buffer_header header;
	struct pspv2_spi_info_request req;
} __packed;

struct pspv2_spi_read_write_request {
	u64 target_nv_id;
	u64 lba;
	u64 offset;
	u64 num_bytes;
	u8 buffer[];
} __packed;

struct mbox_pspv2_cmd_spi_read_write {
	struct mbox_buffer_header header;
	struct pspv2_spi_read_write_request req;
} __packed;

struct pspv2_spi_erase_request {
	u64 target_nv_id;
	u64 lba;
	u64 num_blocks;
} __packed;

struct mbox_pspv2_cmd_spi_erase {
	struct mbox_buffer_header header;
	struct pspv2_spi_erase_request req;
} __packed;

static bool is_valid_psp_spi_id(u64 target_nv_id)
{
	return target_nv_id == SMI_TARGET_NVRAM ||
	       target_nv_id == SMI_TARGET_RPMC_NVRAM;
}

static bool is_valid_psp_spi_info(struct mbox_pspv2_cmd_spi_info *cmd_buf)
{
	return is_valid_psp_spi_id(read64(&cmd_buf->req.target_nv_id));
}

static bool is_valid_psp_spi_read_write(struct mbox_pspv2_cmd_spi_read_write *cmd_buf)
{
	return is_valid_psp_spi_id(read64(&cmd_buf->req.target_nv_id));
}

static bool is_valid_psp_spi_erase(struct mbox_pspv2_cmd_spi_erase *cmd_buf)
{
	return is_valid_psp_spi_id(read64(&cmd_buf->req.target_nv_id));
}

static u64 get_psp_spi_info_id(struct mbox_pspv2_cmd_spi_info *cmd_buf)
{
	return read64(&cmd_buf->req.target_nv_id);
}

static void set_psp_spi_info(struct mbox_pspv2_cmd_spi_info *cmd_buf,
			     u64 lba, u64 block_size, u64 num_blocks)
{
	write64(&cmd_buf->req.lba, lba);
	write64(&cmd_buf->req.block_size, block_size);
	write64(&cmd_buf->req.num_blocks, num_blocks);
}

static void get_psp_spi_read_write(struct mbox_pspv2_cmd_spi_read_write *cmd_buf,
				   u64 *target_nv_id, u64 *lba, u64 *offset,
				   u64 *num_bytes, u8 **data)
{
	*target_nv_id = read64(&cmd_buf->req.target_nv_id);
	*lba = read64(&cmd_buf->req.lba);
	*offset = read64(&cmd_buf->req.offset);
	*num_bytes = read64(&cmd_buf->req.num_bytes);
	*data = cmd_buf->req.buffer;
}

static void get_psp_spi_erase(struct mbox_pspv2_cmd_spi_erase *cmd_buf,
			      u64 *target_nv_id, u64 *lba, u64 *num_blocks)
{
	*target_nv_id = read64(&cmd_buf->req.target_nv_id);
	*lba = read64(&cmd_buf->req.lba);
	*num_blocks = read64(&cmd_buf->req.num_blocks);
}

static const char *id_to_region_name(u64 target_nv_id)
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
static int lookup_store(u64 target_nv_id, struct region_device *rstore)
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

static enum mbox_p2c_status find_psp_spi_flash_device_region(u64 target_nv_id,
							     struct region_device *store,
							     const struct spi_flash **flash)
{
	*flash = boot_device_spi_flash();
	if (*flash == NULL) {
		printk(BIOS_ERR, "PSP: Unable to find SPI device\n");
		return MBOX_PSP_COMMAND_PROCESS_ERROR;
	}

	if (lookup_store(target_nv_id, store) < 0) {
		printk(BIOS_ERR, "PSP: Unable to find PSP SPI region\n");
		return MBOX_PSP_COMMAND_PROCESS_ERROR;
	}

	return MBOX_PSP_SUCCESS;
}

static bool spi_controller_available(void)
{
	return !(spi_read8(SPI_MISC_CNTRL) & SPI_SEMAPHORE_DRIVER_LOCKED);
}

enum mbox_p2c_status psp_smi_spi_get_info(struct mbox_default_buffer *buffer)
{
	struct mbox_pspv2_cmd_spi_info *const cmd_buf =
		(struct mbox_pspv2_cmd_spi_info *)buffer;
	const struct spi_flash *flash;
	struct region_device store;
	u64 target_nv_id;
	u64 block_size;
	u64 num_blocks;
	enum mbox_p2c_status ret;

	printk(BIOS_SPEW, "PSP: SPI info request\n");

	if (!is_valid_psp_spi_info(cmd_buf))
		return MBOX_PSP_COMMAND_PROCESS_ERROR;

	if (!spi_controller_available()) {
		printk(BIOS_NOTICE, "PSP: SPI controller busy\n");
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
	struct mbox_pspv2_cmd_spi_read_write *const cmd_buf =
		(struct mbox_pspv2_cmd_spi_read_write *)buffer;
	enum mbox_p2c_status ret;
	u64 target_nv_id;
	u64 lba;
	u64 offset;
	u64 num_bytes;
	u8 *data;
	size_t addr;
	const struct spi_flash *flash;
	struct region_device store;

	printk(BIOS_SPEW, "PSP: SPI read request\n");

	if (!is_valid_psp_spi_read_write(cmd_buf))
		return MBOX_PSP_COMMAND_PROCESS_ERROR;

	if (!spi_controller_available()) {
		printk(BIOS_NOTICE, "PSP: SPI controller busy\n");
		return MBOX_PSP_SPI_BUSY;
	}

	get_psp_spi_read_write(cmd_buf, &target_nv_id, &lba, &offset, &num_bytes, &data);

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
	struct mbox_pspv2_cmd_spi_read_write *const cmd_buf =
		(struct mbox_pspv2_cmd_spi_read_write *)buffer;
	enum mbox_p2c_status ret;
	u64 target_nv_id;
	u64 lba;
	u64 offset;
	u64 num_bytes;
	u8 *data;
	size_t addr;
	const struct spi_flash *flash;
	struct region_device store;

	printk(BIOS_SPEW, "PSP: SPI write request\n");

	if (!is_valid_psp_spi_read_write(cmd_buf))
		return MBOX_PSP_COMMAND_PROCESS_ERROR;

	if (!spi_controller_available()) {
		printk(BIOS_NOTICE, "PSP: SPI controller busy\n");
		return MBOX_PSP_SPI_BUSY;
	}

	get_psp_spi_read_write(cmd_buf, &target_nv_id, &lba, &offset, &num_bytes, &data);

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
	struct mbox_pspv2_cmd_spi_erase *const cmd_buf =
		(struct mbox_pspv2_cmd_spi_erase *)buffer;
	enum mbox_p2c_status ret;
	u64 target_nv_id;
	u64 lba;
	u64 num_blocks;
	size_t addr;
	size_t num_bytes;
	const struct spi_flash *flash;
	struct region_device store;

	printk(BIOS_SPEW, "PSP: SPI erase request\n");

	if (!is_valid_psp_spi_erase(cmd_buf))
		return MBOX_PSP_COMMAND_PROCESS_ERROR;

	if (!spi_controller_available()) {
		printk(BIOS_NOTICE, "PSP: SPI controller busy\n");
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
