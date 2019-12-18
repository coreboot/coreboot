/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Driver for SST serial flashes
 */

#include <console/console.h>
#include <commonlib/helpers.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <string.h>

#include "spi_flash_internal.h"

#define CMD_SST_WREN		0x06	/* Write Enable */
#define CMD_SST_WRDI		0x04	/* Write Disable */
#define CMD_SST_RDSR		0x05	/* Read Status Register */
#define CMD_SST_WRSR		0x01	/* Write Status Register */
#define CMD_SST_EWSR		0x50	/* Enable Write Status Register */
#define CMD_SST_READ		0x03	/* Read Data Bytes */
#define CMD_SST_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define CMD_SST_BP		0x02	/* Byte Program */
#define CMD_SST_AAI_WP		0xAD	/* Auto Address Increment Word Program */
#define CMD_SST_SE		0x20	/* Sector Erase */

#define SST_SR_WIP		(1 << 0)	/* Write-in-Progress */
#define SST_SR_WEL		(1 << 1)	/* Write enable */
#define SST_SR_BP0		(1 << 2)	/* Block Protection 0 */
#define SST_SR_BP1		(1 << 3)	/* Block Protection 1 */
#define SST_SR_BP2		(1 << 4)	/* Block Protection 2 */
#define SST_SR_AAI		(1 << 6)	/* Addressing mode */
#define SST_SR_BPL		(1 << 7)	/* BP bits lock */

struct sst_spi_flash_params {
	u8 idcode1;
	u16 nr_sectors;
	const char *name;
	const struct spi_flash_ops *ops;
};

static int sst_write_ai(const struct spi_flash *flash, u32 offset, size_t len,
			const void *buf);
static int sst_write_256(const struct spi_flash *flash, u32 offset, size_t len,
			 const void *buf);

static const struct spi_flash_ops spi_flash_ops_write_ai = {
	.write = sst_write_ai,
	.erase = spi_flash_cmd_erase,
	.status = spi_flash_cmd_status,
};

static const struct spi_flash_ops spi_flash_ops_write_256 = {
	.write = sst_write_256,
	.erase = spi_flash_cmd_erase,
	.status = spi_flash_cmd_status,
};

#define SST_SECTOR_SIZE (4 * 1024)
static const struct sst_spi_flash_params sst_spi_flash_table[] = {
	{
		.idcode1 = 0x8d,
		.nr_sectors = 128,
		.name = "SST25VF040B",
		.ops = &spi_flash_ops_write_ai,
	},{
		.idcode1 = 0x8e,
		.nr_sectors = 256,
		.name = "SST25VF080B",
		.ops = &spi_flash_ops_write_ai,
	},{
		.idcode1 = 0x80,
		.nr_sectors = 256,
		.name = "SST25VF080",
		.ops = &spi_flash_ops_write_ai,
	},{
		.idcode1 = 0x41,
		.nr_sectors = 512,
		.name = "SST25VF016B",
		.ops = &spi_flash_ops_write_ai,
	},{
		.idcode1 = 0x4a,
		.nr_sectors = 1024,
		.name = "SST25VF032B",
		.ops = &spi_flash_ops_write_ai,
	},{
		.idcode1 = 0x4b,
		.nr_sectors = 2048,
		.name = "SST25VF064C",
		.ops = &spi_flash_ops_write_256,
	},{
		.idcode1 = 0x01,
		.nr_sectors = 16,
		.name = "SST25WF512",
		.ops = &spi_flash_ops_write_ai,
	},{
		.idcode1 = 0x02,
		.nr_sectors = 32,
		.name = "SST25WF010",
		.ops = &spi_flash_ops_write_ai,
	},{
		.idcode1 = 0x03,
		.nr_sectors = 64,
		.name = "SST25WF020",
		.ops = &spi_flash_ops_write_ai,
	},{
		.idcode1 = 0x04,
		.nr_sectors = 128,
		.name = "SST25WF040",
		.ops = &spi_flash_ops_write_ai,
	},{
		.idcode1 = 0x05,
		.nr_sectors = 256,
		.name = "SST25WF080",
		.ops = &spi_flash_ops_write_ai,
	},{
		.idcode1 = 0x14,
		.nr_sectors = 256,
		.name = "SST25WF080B",
		.ops = &spi_flash_ops_write_ai,
	},
};

static int
sst_enable_writing(const struct spi_flash *flash)
{
	int ret = spi_flash_cmd(&flash->spi, CMD_SST_WREN, NULL, 0);
	if (ret)
		printk(BIOS_WARNING, "SF: Enabling Write failed\n");
	return ret;
}

static int
sst_enable_writing_status(const struct spi_flash *flash)
{
	int ret = spi_flash_cmd(&flash->spi, CMD_SST_EWSR, NULL, 0);
	if (ret)
		printk(BIOS_WARNING, "SF: Enabling Write Status failed\n");
	return ret;
}

static int
sst_disable_writing(const struct spi_flash *flash)
{
	int ret = spi_flash_cmd(&flash->spi, CMD_SST_WRDI, NULL, 0);
	if (ret)
		printk(BIOS_WARNING, "SF: Disabling Write failed\n");
	return ret;
}

static int
sst_byte_write(const struct spi_flash *flash, u32 offset, const void *buf)
{
	int ret;
	u8 cmd[4] = {
		CMD_SST_BP,
		offset >> 16,
		offset >> 8,
		offset,
	};

#if CONFIG(DEBUG_SPI_FLASH)
	printk(BIOS_SPEW, "BP[%02x]: %p => cmd = { 0x%02x 0x%06x }\n",
		spi_w8r8(&flash->spi, CMD_SST_RDSR), buf, cmd[0], offset);
#endif

	ret = sst_enable_writing(flash);
	if (ret)
		return ret;

	ret = spi_flash_cmd_write(&flash->spi, cmd, sizeof(cmd), buf, 1);
	if (ret)
		return ret;

	return spi_flash_cmd_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT_MS);
}

static int sst_write_256(const struct spi_flash *flash, u32 offset, size_t len,
			const void *buf)
{
	size_t actual, chunk_len;
	unsigned long byte_addr;
	unsigned long page_size;
	int ret = 0;
	u8 cmd[4];

	page_size = 256;

	/* If the data is not word aligned, write out leading single byte */
	actual = offset % 2;
	if (actual) {
		ret = sst_byte_write(flash, offset, buf);
		if (ret)
			goto done;
	}
	offset += actual;

	ret = sst_enable_writing(flash);
	if (ret)
		goto done;

	cmd[0] = CMD_SST_AAI_WP;
	cmd[1] = offset >> 16;
	cmd[2] = offset >> 8;
	cmd[3] = offset;

	for (actual = 0; actual < len; actual += chunk_len) {
		byte_addr = offset % page_size;
		chunk_len = MIN(len - actual, page_size - byte_addr);
		chunk_len = spi_crop_chunk(&flash->spi, sizeof(cmd), chunk_len);

		cmd[0] = CMD_SST_BP;
		cmd[1] = (offset >> 16) & 0xff;
		cmd[2] = (offset >> 8) & 0xff;
		cmd[3] = offset & 0xff;
#if CONFIG(DEBUG_SPI_FLASH)
		printk(BIOS_SPEW, "PP: %p => cmd = { 0x%02x 0x%02x%02x%02x }"
		     " chunk_len = %zu\n",
		     buf + actual, cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);
#endif

		ret = spi_flash_cmd(&flash->spi, CMD_SST_WREN, NULL, 0);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: Enabling Write failed\n");
			break;
		}

		ret = spi_flash_cmd_write(&flash->spi, cmd, sizeof(cmd),
					  buf + actual, chunk_len);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: SST Page Program failed\n");
			break;
		}

		ret = spi_flash_cmd_wait_ready(flash,
				SPI_FLASH_PROG_TIMEOUT_MS);
		if (ret)
			break;

		offset += chunk_len;
	}

done:
#if CONFIG(DEBUG_SPI_FLASH)
	printk(BIOS_SPEW, "SF: SST: program %s %zu bytes @ 0x%lx\n",
	      ret ? "failure" : "success", len, (unsigned long)offset - actual);
#endif
	return ret;
}

static int sst_write_ai(const struct spi_flash *flash, u32 offset, size_t len,
			const void *buf)
{
	size_t actual, cmd_len;
	int ret = 0;
	u8 cmd[4];

	/* If the data is not word aligned, write out leading single byte */
	actual = offset % 2;
	if (actual) {
		ret = sst_byte_write(flash, offset, buf);
		if (ret)
			goto done;
	}
	offset += actual;

	ret = sst_enable_writing(flash);
	if (ret)
		goto done;

	cmd_len = 4;
	cmd[0] = CMD_SST_AAI_WP;
	cmd[1] = offset >> 16;
	cmd[2] = offset >> 8;
	cmd[3] = offset;

	for (; actual < len - 1; actual += 2) {
#if CONFIG(DEBUG_SPI_FLASH)
		printk(BIOS_SPEW, "WP[%02x]: %p => cmd = { 0x%02x 0x%06x }\n",
		     spi_w8r8(&flash->spi, CMD_SST_RDSR), buf + actual, cmd[0],
		     offset);
#endif

		ret = spi_flash_cmd_write(&flash->spi, cmd, cmd_len,
		                          buf + actual, 2);
		if (ret) {
			printk(BIOS_WARNING, "SF: SST word program failed\n");
			break;
		}

		ret = spi_flash_cmd_wait_ready(flash,
				SPI_FLASH_PROG_TIMEOUT_MS);
		if (ret)
			break;

		cmd_len = 1;
		offset += 2;
	}

	if (!ret)
		ret = sst_disable_writing(flash);

	/* If there is a single trailing byte, write it out */
	if (!ret && actual != len)
		ret = sst_byte_write(flash, offset, buf + actual);

done:
#if CONFIG(DEBUG_SPI_FLASH)
	printk(BIOS_SPEW, "SF: SST: program %s %zu bytes @ 0x%lx\n",
	      ret ? "failure" : "success", len, (unsigned long)offset - actual);
#endif
	return ret;
}


static int
sst_unlock(const struct spi_flash *flash)
{
	int ret;
	u8 cmd, status;

	ret = sst_enable_writing_status(flash);
	if (ret)
		return ret;

	cmd = CMD_SST_WRSR;
	status = 0;
	ret = spi_flash_cmd_write(&flash->spi, &cmd, 1, &status, 1);
	if (ret)
		printk(BIOS_WARNING, "SF: Unable to set status byte\n");

	printk(BIOS_INFO, "SF: SST: status = %x\n", spi_w8r8(&flash->spi, CMD_SST_RDSR));

	return ret;
}

int spi_flash_probe_sst(const struct spi_slave *spi, u8 *idcode,
			struct spi_flash *flash)
{
	const struct sst_spi_flash_params *params;
	size_t i;

	for (i = 0; i < ARRAY_SIZE(sst_spi_flash_table); ++i) {
		params = &sst_spi_flash_table[i];
		if (params->idcode1 == idcode[2])
			break;
	}

	if (i == ARRAY_SIZE(sst_spi_flash_table)) {
		printk(BIOS_WARNING, "SF: Unsupported SST ID %02x\n", idcode[1]);
		return -1;
	}

	memcpy(&flash->spi, spi, sizeof(*spi));
	flash->name = params->name;
	flash->sector_size = SST_SECTOR_SIZE;
	flash->size = flash->sector_size * params->nr_sectors;
	flash->erase_cmd = CMD_SST_SE;
	flash->status_cmd = CMD_SST_RDSR;

	flash->ops = params->ops;

	/* Flash powers up read-only, so clear BP# bits */
	sst_unlock(flash);

	return 0;
}
