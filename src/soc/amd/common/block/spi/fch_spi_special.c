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

#include <console/console.h>
#include <spi-generic.h>
#include <amdblocks/fch_spi.h>

int non_standard_sst_byte_write(u32 offset, const void *buf)
{
	int ret;
	u8 cmd[4] = {
		CMD_SST_BP,
		offset >> 16,
		offset >> 8,
		offset,
	};

	ret = fch_spi_enable_write();
	if (ret)
		return ret;

	ret = fch_spi_flash_cmd_write(cmd, sizeof(cmd), buf, 1);
	if (ret)
		return ret;

	return fch_spi_wait_cmd_ready(SPI_FLASH_PROG_TIMEOUT_MS);
}

int non_standard_sst_write_aai(u32 offset, size_t len, const void *buf, size_t start)
{
	size_t actual, cmd_len;
	int ret = 0;
	u8 cmd[4];

	ret = fch_spi_enable_write();
	if (ret)
		goto done;

	cmd_len = 4;
	cmd[0] = CMD_SST_AAI_WP;
	cmd[1] = offset >> 16;
	cmd[2] = offset >> 8;
	cmd[3] = offset;

	for (actual = start; actual < len - 1; actual += 2) {
#if CONFIG(SOC_AMD_COMMON_BLOCK_SPI_DEBUG)
		printk(BIOS_DEBUG, "PP: %p => cmd = { 0x%02x 0x%06lx }"
		     " chunk_len = 2\n",
		     buf + actual, cmd[0], (offset + actual));
#endif

		ret = fch_spi_enable_write();
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: Enabling Write failed\n");
			break;
		}

		ret = fch_spi_flash_cmd_write(cmd, cmd_len, buf + actual, 2);
		if (ret < 0) {
			printk(BIOS_WARNING, "SF: SST word Program failed\n");
			break;
		}

		ret = fch_spi_wait_cmd_ready(SPI_FLASH_PROG_TIMEOUT_MS);
		if (ret)
			break;

		offset += 2;
		cmd_len = 1;
	}
	/* If there is a single trailing byte, write it out */
	if (!ret && actual != len)
		ret = non_standard_sst_byte_write(offset, buf + actual);
done:
	return ret;
}
