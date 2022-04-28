/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <boot/coreboot_tables.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <string.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <timer.h>
#include <types.h>

#include "spi_flash_internal.h"

static void spi_flash_addr(u32 addr, u8 *cmd)
{
	/* cmd[0] is actual command */
	cmd[1] = addr >> 16;
	cmd[2] = addr >> 8;
	cmd[3] = addr >> 0;
}

static int do_spi_flash_cmd(const struct spi_slave *spi, const u8 *dout,
			    size_t bytes_out, void *din, size_t bytes_in)
{
	int ret;
	/*
	 * SPI flash requires command-response kind of behavior. Thus, two
	 * separate SPI vectors are required -- first to transmit dout and other
	 * to receive in din. If some specialized SPI flash controllers
	 * (e.g. x86) can perform both command and response together, it should
	 * be handled at SPI flash controller driver level.
	 */
	struct spi_op vectors[] = {
		[0] = { .dout = dout, .bytesout = bytes_out,
			.din = NULL, .bytesin = 0, },
		[1] = { .dout = NULL, .bytesout = 0,
			.din = din, .bytesin = bytes_in },
	};
	size_t count = ARRAY_SIZE(vectors);
	if (!bytes_in)
		count = 1;

	ret = spi_claim_bus(spi);
	if (ret)
		return ret;

	ret = spi_xfer_vector(spi, vectors, count);

	spi_release_bus(spi);
	return ret;
}

static int do_dual_output_cmd(const struct spi_slave *spi, const u8 *dout,
			      size_t bytes_out, void *din, size_t bytes_in)
{
	int ret;

	/*
	 * spi_xfer_vector() will automatically fall back to .xfer() if
	 * .xfer_vector() is unimplemented. So using vector API here is more
	 * flexible, even though a controller that implements .xfer_vector()
	 * and (the non-vector based) .xfer_dual() but not .xfer() would be
	 * pretty odd.
	 */
	struct spi_op vector = { .dout = dout, .bytesout = bytes_out,
				 .din = NULL, .bytesin = 0 };

	ret = spi_claim_bus(spi);
	if (ret)
		return ret;

	ret = spi_xfer_vector(spi, &vector, 1);

	if (!ret)
		ret = spi->ctrlr->xfer_dual(spi, NULL, 0, din, bytes_in);

	spi_release_bus(spi);
	return ret;
}

static int do_dual_io_cmd(const struct spi_slave *spi, const u8 *dout,
			  size_t bytes_out, void *din, size_t bytes_in)
{
	int ret;

	/* Only the very first byte (opcode) is transferred in "single" mode. */
	struct spi_op vector = { .dout = dout, .bytesout = 1,
				 .din = NULL, .bytesin = 0 };

	ret = spi_claim_bus(spi);
	if (ret)
		return ret;

	ret = spi_xfer_vector(spi, &vector, 1);

	if (!ret)
		ret = spi->ctrlr->xfer_dual(spi, &dout[1], bytes_out - 1, NULL, 0);

	if (!ret)
		ret = spi->ctrlr->xfer_dual(spi, NULL, 0, din, bytes_in);

	spi_release_bus(spi);
	return ret;
}

int spi_flash_cmd(const struct spi_slave *spi, u8 cmd, void *response, size_t len)
{
	int ret = do_spi_flash_cmd(spi, &cmd, sizeof(cmd), response, len);
	if (ret)
		printk(BIOS_WARNING, "SF: Failed to send command %02x: %d\n", cmd, ret);

	return ret;
}

/* TODO: This code is quite possibly broken and overflowing stacks. Fix ASAP! */
#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wstack-usage="
#endif
#pragma GCC diagnostic ignored "-Wvla"
int spi_flash_cmd_write(const struct spi_slave *spi, const u8 *cmd,
			size_t cmd_len, const void *data, size_t data_len)
{
	int ret;
	u8 buff[cmd_len + data_len];
	memcpy(buff, cmd, cmd_len);
	memcpy(buff + cmd_len, data, data_len);

	ret = do_spi_flash_cmd(spi, buff, cmd_len + data_len, NULL, 0);
	if (ret) {
		printk(BIOS_WARNING, "SF: Failed to send write command (%zu bytes): %d\n",
				data_len, ret);
	}

	return ret;
}
#pragma GCC diagnostic pop

/* Perform the read operation honoring spi controller fifo size, reissuing
 * the read command until the full request completed. */
int spi_flash_cmd_read(const struct spi_flash *flash, u32 offset,
				  size_t len, void *buf)
{
	u8 cmd[5];
	int ret, cmd_len;
	int (*do_cmd)(const struct spi_slave *spi, const u8 *din,
		      size_t in_bytes, void *out, size_t out_bytes);

	if (CONFIG(SPI_FLASH_NO_FAST_READ)) {
		cmd_len = 4;
		cmd[0] = CMD_READ_ARRAY_SLOW;
		do_cmd = do_spi_flash_cmd;
	} else if (flash->flags.dual_io && flash->spi.ctrlr->xfer_dual) {
		cmd_len = 5;
		cmd[0] = CMD_READ_FAST_DUAL_IO;
		cmd[4] = 0;
		do_cmd = do_dual_io_cmd;
	} else if (flash->flags.dual_output && flash->spi.ctrlr->xfer_dual) {
		cmd_len = 5;
		cmd[0] = CMD_READ_FAST_DUAL_OUTPUT;
		cmd[4] = 0;
		do_cmd = do_dual_output_cmd;
	} else {
		cmd_len = 5;
		cmd[0] = CMD_READ_ARRAY_FAST;
		cmd[4] = 0;
		do_cmd = do_spi_flash_cmd;
	}

	uint8_t *data = buf;
	while (len) {
		size_t xfer_len = spi_crop_chunk(&flash->spi, cmd_len, len);
		spi_flash_addr(offset, cmd);
		ret = do_cmd(&flash->spi, cmd, cmd_len, data, xfer_len);
		if (ret) {
			printk(BIOS_WARNING,
			       "SF: Failed to send read command %#.2x(%#x, %#zx): %d\n",
			       cmd[0], offset, xfer_len, ret);
			return ret;
		}
		offset += xfer_len;
		data += xfer_len;
		len -= xfer_len;
	}

	return 0;
}

int spi_flash_cmd_poll_bit(const struct spi_flash *flash, unsigned long timeout,
			   u8 cmd, u8 poll_bit)
{
	const struct spi_slave *spi = &flash->spi;
	int ret;
	u8 status;
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, timeout);
	do {
		ret = do_spi_flash_cmd(spi, &cmd, 1, &status, 1);
		if (ret)
			return -1;
		if ((status & poll_bit) == 0)
			return 0;
	} while (!stopwatch_expired(&sw));

	printk(BIOS_WARNING, "SF: timeout at %ld msec\n", stopwatch_duration_msecs(&sw));

	return -1;
}

int spi_flash_cmd_wait_ready(const struct spi_flash *flash,
			unsigned long timeout)
{
	return spi_flash_cmd_poll_bit(flash, timeout,
		CMD_READ_STATUS, STATUS_WIP);
}

int spi_flash_cmd_erase(const struct spi_flash *flash, u32 offset, size_t len)
{
	u32 start, end, erase_size;
	int ret = -1;
	u8 cmd[4];

	erase_size = flash->sector_size;
	if (offset % erase_size || len % erase_size) {
		printk(BIOS_WARNING, "SF: Erase offset/length not multiple of erase size\n");
		return -1;
	}
	if (len == 0) {
		printk(BIOS_WARNING, "SF: Erase length cannot be 0\n");
		return -1;
	}

	cmd[0] = flash->erase_cmd;
	start = offset;
	end = start + len;

	while (offset < end) {
		spi_flash_addr(offset, cmd);
		offset += erase_size;

#if CONFIG(DEBUG_SPI_FLASH)
		printk(BIOS_SPEW, "SF: erase %2x %2x %2x %2x (%x)\n", cmd[0], cmd[1],
		      cmd[2], cmd[3], offset);
#endif
		ret = spi_flash_cmd(&flash->spi, CMD_WRITE_ENABLE, NULL, 0);
		if (ret)
			goto out;

		ret = spi_flash_cmd_write(&flash->spi, cmd, sizeof(cmd), NULL, 0);
		if (ret)
			goto out;

		ret = spi_flash_cmd_wait_ready(flash,
				SPI_FLASH_PAGE_ERASE_TIMEOUT_MS);
		if (ret)
			goto out;
	}

	printk(BIOS_DEBUG, "SF: Successfully erased %zu bytes @ %#x\n", len, start);

out:
	return ret;
}

int spi_flash_cmd_status(const struct spi_flash *flash, u8 *reg)
{
	return spi_flash_cmd(&flash->spi, flash->status_cmd, reg, sizeof(*reg));
}

int spi_flash_cmd_write_page_program(const struct spi_flash *flash, u32 offset,
				size_t len, const void *buf)
{
	unsigned long byte_addr;
	unsigned long page_size;
	size_t chunk_len;
	size_t actual;
	int ret = 0;
	u8 cmd[4];

	page_size = flash->page_size;
	cmd[0] = flash->pp_cmd;

	for (actual = 0; actual < len; actual += chunk_len) {
		byte_addr = offset % page_size;
		chunk_len = MIN(len - actual, page_size - byte_addr);
		chunk_len = spi_crop_chunk(&flash->spi, sizeof(cmd), chunk_len);

		spi_flash_addr(offset, cmd);
		if (CONFIG(DEBUG_SPI_FLASH)) {
			printk(BIOS_SPEW, "PP: %p => cmd = { 0x%02x 0x%02x%02x%02x } chunk_len = %zu\n",
				buf + actual, cmd[0], cmd[1], cmd[2], cmd[3],
				chunk_len);
		}

		ret = spi_flash_cmd(&flash->spi, flash->wren_cmd, NULL, 0);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: Enabling Write failed\n");
			goto out;
		}

		ret = spi_flash_cmd_write(&flash->spi, cmd, sizeof(cmd),
				buf + actual, chunk_len);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: Page Program failed\n");
			goto out;
		}

		ret = spi_flash_cmd_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT_MS);
		if (ret)
			goto out;

		offset += chunk_len;
	}

	if (CONFIG(DEBUG_SPI_FLASH))
		printk(BIOS_SPEW, "SF: : Successfully programmed %zu bytes @ 0x%lx\n",
			len, (unsigned long)(offset - len));
	ret = 0;

out:
	return ret;
}

static const struct spi_flash_vendor_info *spi_flash_vendors[] = {
#if CONFIG(SPI_FLASH_ADESTO)
	&spi_flash_adesto_vi,
#endif
#if CONFIG(SPI_FLASH_AMIC)
	&spi_flash_amic_vi,
#endif
#if CONFIG(SPI_FLASH_ATMEL)
	&spi_flash_atmel_vi,
#endif
#if CONFIG(SPI_FLASH_EON)
	&spi_flash_eon_vi,
#endif
#if CONFIG(SPI_FLASH_GIGADEVICE)
	&spi_flash_gigadevice_vi,
#endif
#if CONFIG(SPI_FLASH_MACRONIX)
	&spi_flash_macronix_vi,
#endif
#if CONFIG(SPI_FLASH_SPANSION)
	&spi_flash_spansion_ext1_vi,
	&spi_flash_spansion_ext2_vi,
	&spi_flash_spansion_vi,
#endif
#if CONFIG(SPI_FLASH_SST)
	&spi_flash_sst_ai_vi,
	&spi_flash_sst_vi,
#endif
#if CONFIG(SPI_FLASH_STMICRO)
	&spi_flash_stmicro1_vi,
	&spi_flash_stmicro2_vi,
	&spi_flash_stmicro3_vi,
	&spi_flash_stmicro4_vi,
#endif
#if CONFIG(SPI_FLASH_WINBOND)
	&spi_flash_winbond_vi,
#endif
};
#define IDCODE_LEN 5

static int fill_spi_flash(const struct spi_slave *spi, struct spi_flash *flash,
	const struct spi_flash_vendor_info *vi,
	const struct spi_flash_part_id *part)
{
	memcpy(&flash->spi, spi, sizeof(*spi));
	flash->vendor = vi->id;
	flash->model = part->id[0];

	flash->page_size = 1U << vi->page_size_shift;
	flash->sector_size = (1U << vi->sector_size_kib_shift) * KiB;
	flash->size = flash->sector_size * (1U << part->nr_sectors_shift);
	flash->erase_cmd = vi->desc->erase_cmd;
	flash->status_cmd = vi->desc->status_cmd;
	flash->pp_cmd = vi->desc->pp_cmd;
	flash->wren_cmd = vi->desc->wren_cmd;

	flash->flags.dual_output = part->fast_read_dual_output_support;
	flash->flags.dual_io = part->fast_read_dual_io_support;

	flash->ops = &vi->desc->ops;
	flash->prot_ops = vi->prot_ops;
	flash->part = part;

	if (vi->after_probe)
		return vi->after_probe(flash);

	return 0;
}

static const struct spi_flash_part_id *find_part(const struct spi_flash_vendor_info *vi,
						uint16_t id[2])
{
	size_t i;
	const uint16_t lid[2] = {
		[0] = id[0] & vi->match_id_mask[0],
		[1] = id[1] & vi->match_id_mask[1],
	};

	for (i = 0; i < vi->nr_part_ids; i++) {
		const struct spi_flash_part_id *part = &vi->ids[i];

		if (part->id[0] == lid[0] && part->id[1] == lid[1])
			return part;
	}

	return NULL;
}

static int find_match(const struct spi_slave *spi, struct spi_flash *flash,
			uint8_t manuf_id, uint16_t id[2])
{
	int i;

	for (i = 0; i < (int)ARRAY_SIZE(spi_flash_vendors); i++) {
		const struct spi_flash_vendor_info *vi;
		const struct spi_flash_part_id *part;

		vi = spi_flash_vendors[i];

		if (manuf_id != vi->id)
			continue;

		part = find_part(vi, id);

		if (part == NULL)
			continue;

		return fill_spi_flash(spi, flash, vi, part);
	}

	return -1;
}

int spi_flash_generic_probe(const struct spi_slave *spi,
				struct spi_flash *flash)
{
	int ret, i;
	u8 idcode[IDCODE_LEN];
	u8 manuf_id;
	u16 id[2];

	/* Read the ID codes */
	ret = spi_flash_cmd(spi, CMD_READ_ID, idcode, sizeof(idcode));
	if (ret)
		return -1;

	if (CONFIG(DEBUG_SPI_FLASH)) {
		printk(BIOS_SPEW, "SF: Got idcode: ");
		for (i = 0; i < sizeof(idcode); i++)
			printk(BIOS_SPEW, "%02x ", idcode[i]);
		printk(BIOS_SPEW, "\n");
	}

	manuf_id = idcode[0];

	printk(BIOS_INFO, "Manufacturer: %02x\n", manuf_id);

	/* If no result from RDID command and STMicro parts are enabled attempt
	   to wake the part from deep sleep and obtain alternative id info. */
	if (CONFIG(SPI_FLASH_STMICRO) && manuf_id == 0xff) {
		if (stmicro_release_deep_sleep_identify(spi, idcode))
			return -1;
		manuf_id = idcode[0];
	}

	id[0] = (idcode[1] << 8) | idcode[2];
	id[1] = (idcode[3] << 8) | idcode[4];

	return find_match(spi, flash, manuf_id, id);
}

int spi_flash_probe(unsigned int bus, unsigned int cs, struct spi_flash *flash)
{
	struct spi_slave spi;
	int ret = -1;

	if (spi_setup_slave(bus, cs, &spi)) {
		printk(BIOS_WARNING, "SF: Failed to set up slave\n");
		return -1;
	}

	/* Try special programmer probe if any. */
	if (spi.ctrlr->flash_probe)
		ret = spi.ctrlr->flash_probe(&spi, flash);

	/* If flash is not found, try generic spi flash probe. */
	if (ret)
		ret = spi_flash_generic_probe(&spi, flash);

	/* Give up -- nothing more to try if flash is not found. */
	if (ret) {
		printk(BIOS_WARNING, "SF: Unsupported manufacturer!\n");
		return -1;
	}

	const char *mode_string = "";
	if (flash->flags.dual_io && spi.ctrlr->xfer_dual)
		mode_string = " (Dual I/O mode)";
	else if (flash->flags.dual_output && spi.ctrlr->xfer_dual)
		mode_string = " (Dual Output mode)";
	printk(BIOS_INFO,
	       "SF: Detected %02x %04x with sector size 0x%x, total 0x%x%s\n",
		flash->vendor, flash->model, flash->sector_size, flash->size, mode_string);
	if (bus == CONFIG_BOOT_DEVICE_SPI_FLASH_BUS
			&& flash->size != CONFIG_ROM_SIZE) {
		printk(BIOS_ERR, "SF size 0x%x does not correspond to"
			" CONFIG_ROM_SIZE 0x%x!!\n", flash->size,
			CONFIG_ROM_SIZE);
	}

	if (CONFIG(SPI_FLASH_EXIT_4_BYTE_ADDR_MODE) && ENV_INITIAL_STAGE)
		spi_flash_cmd(&flash->spi, CMD_EXIT_4BYTE_ADDR_MODE, NULL, 0);

	return 0;
}

int spi_flash_read(const struct spi_flash *flash, u32 offset, size_t len,
		void *buf)
{
	return flash->ops->read(flash, offset, len, buf);
}

int spi_flash_write(const struct spi_flash *flash, u32 offset, size_t len,
		const void *buf)
{
	int ret;

	if (spi_flash_volatile_group_begin(flash))
		return -1;

	ret = flash->ops->write(flash, offset, len, buf);

	if (spi_flash_volatile_group_end(flash))
		return -1;

	return ret;
}

int spi_flash_erase(const struct spi_flash *flash, u32 offset, size_t len)
{
	int ret;

	if (spi_flash_volatile_group_begin(flash))
		return -1;

	ret = flash->ops->erase(flash, offset, len);

	if (spi_flash_volatile_group_end(flash))
		return -1;

	return ret;
}

int spi_flash_status(const struct spi_flash *flash, u8 *reg)
{
	if (flash->ops->status)
		return flash->ops->status(flash, reg);

	return -1;
}

int spi_flash_is_write_protected(const struct spi_flash *flash,
				 const struct region *region)
{
	struct region flash_region = { 0 };

	if (!flash || !region)
		return -1;

	flash_region.size = flash->size;

	if (!region_is_subregion(&flash_region, region))
		return -1;

	if (!flash->prot_ops) {
		printk(BIOS_WARNING, "SPI: Write-protection gathering not "
		       "implemented for this vendor.\n");
		return -1;
	}

	return flash->prot_ops->get_write(flash, region);
}

int spi_flash_set_write_protected(const struct spi_flash *flash,
				  const struct region *region,
				  const enum spi_flash_status_reg_lockdown mode)
{
	struct region flash_region = { 0 };
	int ret;

	if (!flash)
		return -1;

	flash_region.size = flash->size;

	if (!region_is_subregion(&flash_region, region))
		return -1;

	if (!flash->prot_ops) {
		printk(BIOS_WARNING, "SPI: Setting write-protection is not "
		       "implemented for this vendor.\n");
		return -1;
	}

	ret = flash->prot_ops->set_write(flash, region, mode);

	if (ret == 0 && mode != SPI_WRITE_PROTECTION_PRESERVE) {
		printk(BIOS_INFO, "SPI: SREG lock-down was set to ");
		switch (mode) {
		case SPI_WRITE_PROTECTION_NONE:
			printk(BIOS_INFO, "NEVER\n");
		break;
		case SPI_WRITE_PROTECTION_PIN:
			printk(BIOS_INFO, "WP\n");
		break;
		case SPI_WRITE_PROTECTION_REBOOT:
			printk(BIOS_INFO, "REBOOT\n");
		break;
		case SPI_WRITE_PROTECTION_PERMANENT:
			printk(BIOS_INFO, "PERMANENT\n");
		break;
		default:
			printk(BIOS_INFO, "UNKNOWN\n");
		break;
		}
	}

	return ret;
}

static uint32_t volatile_group_count;

int spi_flash_volatile_group_begin(const struct spi_flash *flash)
{
	uint32_t count;
	int ret = 0;

	if (!CONFIG(SPI_FLASH_HAS_VOLATILE_GROUP))
		return ret;

	count = volatile_group_count;
	if (count == 0)
		ret = chipset_volatile_group_begin(flash);

	count++;
	volatile_group_count = count;
	return ret;
}

int spi_flash_volatile_group_end(const struct spi_flash *flash)
{
	uint32_t count;
	int ret = 0;

	if (!CONFIG(SPI_FLASH_HAS_VOLATILE_GROUP))
		return ret;

	count = volatile_group_count;
	assert(count == 0);
	count--;
	volatile_group_count = count;

	if (count == 0)
		ret = chipset_volatile_group_end(flash);

	return ret;
}

void lb_spi_flash(struct lb_header *header)
{
	struct lb_spi_flash *flash;
	const struct spi_flash *spi_flash_dev;

	if (!CONFIG(BOOT_DEVICE_SPI_FLASH))
		return;

	flash = (struct lb_spi_flash *)lb_new_record(header);

	flash->tag = LB_TAG_SPI_FLASH;
	flash->size = sizeof(*flash);

	spi_flash_dev = boot_device_spi_flash();

	if (spi_flash_dev) {
		flash->flash_size = spi_flash_dev->size;
		flash->sector_size = spi_flash_dev->sector_size;
		flash->erase_cmd = spi_flash_dev->erase_cmd;
	} else {
		flash->flash_size = CONFIG_ROM_SIZE;
		/* Default 64k erase command should work on most flash.
		 * Uniform 4k erase only works on certain devices. */
		flash->sector_size = 64 * KiB;
		flash->erase_cmd = CMD_BLOCK_ERASE;
	}

	if (!CONFIG(BOOT_DEVICE_MEMORY_MAPPED)) {
		flash->mmap_count = 0;
	} else {
		struct flash_mmap_window *table = (struct flash_mmap_window *)(flash + 1);
		flash->mmap_count = spi_flash_get_mmap_windows(table);
		flash->size += flash->mmap_count * sizeof(*table);
	}
}

int spi_flash_ctrlr_protect_region(const struct spi_flash *flash,
				   const struct region *region,
				   const enum ctrlr_prot_type type)
{
	const struct spi_ctrlr *ctrlr;
	struct region flash_region = { 0 };

	if (!flash)
		return -1;

	flash_region.size = flash->size;

	if (!region_is_subregion(&flash_region, region))
		return -1;

	ctrlr = flash->spi.ctrlr;

	if (!ctrlr)
		return -1;

	if (ctrlr->flash_protect)
		return ctrlr->flash_protect(flash, region, type);

	return -1;
}

int spi_flash_vector_helper(const struct spi_slave *slave,
	struct spi_op vectors[], size_t count,
	int (*func)(const struct spi_slave *slave, const void *dout,
		    size_t bytesout, void *din, size_t bytesin))
{
	int ret;
	void *din;
	size_t bytes_in;

	if (count < 1 || count > 2)
		return -1;

	/* SPI flash commands always have a command first... */
	if (!vectors[0].dout || !vectors[0].bytesout)
		return -1;
	/* And not read any data during the command. */
	if (vectors[0].din || vectors[0].bytesin)
		return -1;

	if (count == 2) {
		/* If response bytes requested ensure the buffer is valid. */
		if (vectors[1].bytesin && !vectors[1].din)
			return -1;
		/* No sends can accompany a receive. */
		if (vectors[1].dout || vectors[1].bytesout)
			return -1;
		din = vectors[1].din;
		bytes_in = vectors[1].bytesin;
	} else {
		din = NULL;
		bytes_in = 0;
	}

	ret = func(slave, vectors[0].dout, vectors[0].bytesout, din, bytes_in);

	if (ret) {
		vectors[0].status = SPI_OP_FAILURE;
		if (count == 2)
			vectors[1].status = SPI_OP_FAILURE;
	} else {
		vectors[0].status = SPI_OP_SUCCESS;
		if (count == 2)
			vectors[1].status = SPI_OP_SUCCESS;
	}

	return ret;
}

const struct spi_flash_ops_descriptor spi_flash_pp_0x20_sector_desc = {
	.erase_cmd = 0x20, /* Sector Erase */
	.status_cmd = 0x05, /* Read Status */
	.pp_cmd = 0x02, /* Page Program */
	.wren_cmd = 0x06, /* Write Enable */
	.ops = {
		.read = spi_flash_cmd_read,
		.write = spi_flash_cmd_write_page_program,
		.erase = spi_flash_cmd_erase,
		.status = spi_flash_cmd_status,
	},
};

const struct spi_flash_ops_descriptor spi_flash_pp_0xd8_sector_desc = {
	.erase_cmd = 0xd8, /* Sector Erase */
	.status_cmd = 0x05, /* Read Status */
	.pp_cmd = 0x02, /* Page Program */
	.wren_cmd = 0x06, /* Write Enable */
	.ops = {
		.read = spi_flash_cmd_read,
		.write = spi_flash_cmd_write_page_program,
		.erase = spi_flash_cmd_erase,
		.status = spi_flash_cmd_status,
	},
};
