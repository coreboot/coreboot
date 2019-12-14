/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Silverback Ltd.
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
#include <console/console.h>
#include <spi_flash.h>
#include <soc/southbridge.h>
#include <amdblocks/fch_spi.h>
#include <drivers/spi/spi_flash_internal.h>
#include <timer.h>
#include <string.h>

static void spi_flash_addr(u32 addr, u8 *cmd)
{
	/* cmd[0] is actual command */
	cmd[1] = addr >> 16;
	cmd[2] = addr >> 8;
	cmd[3] = addr >> 0;
}

static int crop_chunk(unsigned int cmd_len, unsigned int buf_len)
{
	return MIN((SPI_FIFO_DEPTH - (cmd_len - 1)), buf_len);
}

int fch_spi_flash_cmd_write(const u8 *cmd, size_t cmd_len, const void *data, size_t data_len)
{
	int ret;
	u8 buff[SPI_FIFO_DEPTH + 1];

	/* Ensure FIFO is large enough.  First byte of command does not go in the FIFO. */
	if ((cmd_len - 1 + data_len) > SPI_FIFO_DEPTH)
		return -1;
	memcpy(buff, cmd, cmd_len);
	memcpy(buff + cmd_len, data, data_len);

	ret = fch_spi_flash_cmd(buff, cmd_len + data_len, NULL, 0);
	if (ret) {
		printk(BIOS_WARNING, "FCH_SF: Failed to send write command (%zu bytes): %d\n",
				data_len, ret);
	}

	return ret;
}

static int fch_spi_flash_status(const struct spi_flash *flash, uint8_t *reg)
{
	int ret;
	u8 status, cmd = CMD_READ_STATUS;

	ret = fch_spi_flash_cmd(&cmd, 1, &status, 1);
	if (!ret)
		*reg = status;
	return ret;
}

int fch_spi_wait_cmd_ready(unsigned long timeout)
{
	struct mono_time current, end;
	int ret;
	u8 status;

	timer_monotonic_get(&current);
	end = current;
	mono_time_add_msecs(&end, timeout);

	do {
		ret = fch_spi_flash_status(NULL, &status);
		if (ret)
			return -1;
		if ((status & STATUS_WIP) == 0)
			return 0;
		timer_monotonic_get(&current);
	} while (!mono_time_after(&current, &end));

	printk(BIOS_DEBUG, "FCH_SF: timeout at %ld msec\n", timeout);
	return -1;
}

static int fch_spi_flash_erase(const struct spi_flash *flash, uint32_t offset, size_t len)
{
	u32 start, end, erase_size;
	const struct spi_data *spi_data_ptr = get_ctrl_spi_data();
	int ret = -1;
	u8 cmd[4];

	erase_size = spi_data_ptr->sector_size;
	if (offset % erase_size || len % erase_size) {
		printk(BIOS_WARNING, "%s: Erase offset/length not multiple of erase size\n",
					spi_data_ptr->name);
		return -1;
	}
	if (len == 0) {
		printk(BIOS_WARNING, "%s: Erase length cannot be 0\n", spi_data_ptr->name);
		return -1;
	}

	cmd[0] = spi_data_ptr->erase_cmd;
	start = offset;
	end = start + len;

	while (offset < end) {
		spi_flash_addr(offset, cmd);
		offset += erase_size;

#if CONFIG(SOC_AMD_COMMON_BLOCK_SPI_DEBUG)
		printk(BIOS_DEBUG, "FCH_SF: erase %2x %2x %2x %2x (%x)\n", cmd[0], cmd[1],
								cmd[2], cmd[3], offset);
#endif
		ret = fch_spi_enable_write();
		if (ret)
			goto out;

		ret = fch_spi_flash_cmd_write(cmd, sizeof(cmd), NULL, 0);
		if (ret)
			goto out;

		ret = fch_spi_wait_cmd_ready(SPI_FLASH_PAGE_ERASE_TIMEOUT_MS);
		if (ret)
			goto out;
	}

	printk(BIOS_DEBUG, "%s: Successfully erased %zu bytes @ %#x\n", spi_data_ptr->name, len,
											start);

out:
	return ret;
}

static int fch_spi_flash_read(const struct spi_flash *flash, uint32_t offset, size_t len,
			      void *buf)
{
	const struct spi_data *spi_data_ptr = get_ctrl_spi_data();
	uint8_t *data = buf;
	int ret;
	size_t xfer_len;
	u8 cmd[5];

	cmd[0] = spi_data_ptr->read_cmd;
	cmd[4] = 0;
	while (len) {
		xfer_len = crop_chunk(spi_data_ptr->read_cmd_len, len);
		spi_flash_addr(offset, cmd);
		ret = fch_spi_flash_cmd(cmd, spi_data_ptr->read_cmd_len, data, xfer_len);
		if (ret) {
			printk(BIOS_WARNING,
			       "FCH_SF: Failed to send read command %#.2x(%#x, %#zx): %d\n",
			       cmd[0], offset, xfer_len, ret);
			return ret;
		}
		offset += xfer_len;
		data += xfer_len;
		len -= xfer_len;
	}
	return 0;
}

static int fch_spi_flash_write(const struct spi_flash *flash, uint32_t offset, size_t len,
			      const void *buf)
{
	unsigned long byte_addr;
	unsigned long page_size;
	const struct spi_data *spi_data_ptr = get_ctrl_spi_data();
	size_t chunk_len;
	size_t actual, start = 0;
	int ret = 0;
	u8 cmd[4];

	page_size = spi_data_ptr->page_size;
	if (spi_data_ptr->non_standard == NON_STANDARD_SPI_SST) {
		if (offset % 2) {
			ret = non_standard_sst_byte_write(offset, buf);
			len--;
			start++;
			offset++;
			if (ret)
				return ret;
		}
		if (page_size == 2)
			return non_standard_sst_write_aai(offset, len, buf, start);
	}

	for (actual = start; actual < len; actual += chunk_len) {
		byte_addr = offset % page_size;
		chunk_len = MIN(len - actual, page_size - byte_addr);
		chunk_len = crop_chunk(sizeof(cmd), chunk_len);

		cmd[0] = spi_data_ptr->write_cmd;
		cmd[1] = (offset >> 16) & 0xff;
		cmd[2] = (offset >> 8) & 0xff;
		cmd[3] = offset & 0xff;
#if CONFIG(SOC_AMD_COMMON_BLOCK_SPI_DEBUG)
		printk(BIOS_DEBUG, "PP: %p => cmd = { 0x%02x 0x%02x%02x%02x } chunk_len = %zu"
				"\n", buf + actual, cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);
#endif

		ret = fch_spi_enable_write();
		if (ret < 0) {
			printk(BIOS_WARNING, "%s: Enabling Write failed\n", spi_data_ptr->name);
			goto out;
		}

		ret = fch_spi_flash_cmd_write(cmd, sizeof(cmd), buf + actual, chunk_len);
		if (ret < 0) {
			printk(BIOS_WARNING, "%s: Page Program failed\n", spi_data_ptr->name);
			goto out;
		}

		ret = fch_spi_wait_cmd_ready(SPI_FLASH_PROG_TIMEOUT_MS);
		if (ret)
			goto out;

		offset += chunk_len;
	}

#if CONFIG(SOC_AMD_COMMON_BLOCK_SPI_DEBUG)
	printk(BIOS_DEBUG, "%s: Successfully programmed %zu bytes @ 0x%lx\n",
				spi_data_ptr->name, len, (unsigned long)(offset - len));
#endif
	ret = 0;

out:
	return ret;
}

static const struct spi_flash_ops fch_spi_flash_ops = {
	.read = fch_spi_flash_read,
	.write = fch_spi_flash_write,
	.erase = fch_spi_flash_erase,
	.status = fch_spi_flash_status,
};

void fch_spi_flash_ops_init(struct spi_flash *flash)
{
	flash->ops = &fch_spi_flash_ops;
}
