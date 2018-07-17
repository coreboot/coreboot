/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

/* NOR Flash is clocked with 26MHz, from CLK26M -> TOP_SPINFI_IFR */

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <symbols.h>
#include <timer.h>
#include <soc/symbols.h>
#include <soc/flash_controller.h>

#define get_nth_byte(d, n)	((d >> (8 * n)) & 0xff)

static int polling_cmd(u32 val)
{
	struct stopwatch sw;

	stopwatch_init_usecs_expire(&sw, SFLASH_POLLINGREG_US);

	while ((read32(&mt8173_nor->cmd) & val) != 0) {
		if (stopwatch_expired(&sw))
			return -1;
	}

	return 0;
}

static int mt8173_nor_execute_cmd(u8 cmdval)
{
	u8 val = cmdval & ~(SFLASH_AUTOINC);

	write8(&mt8173_nor->cmd, cmdval);
	return polling_cmd(val);
}

static int sflashhw_read_flash_status(u8 *value)
{
	if (mt8173_nor_execute_cmd(SFLASH_READSTATUS))
		return -1;

	*value = read8(&mt8173_nor->rdsr);
	return 0;
}

static int wait_for_write_done(void)
{
	struct stopwatch sw;
	u8 reg;

	stopwatch_init_usecs_expire(&sw, SFLASH_POLLINGREG_US);

	while (sflashhw_read_flash_status(&reg) == 0) {
		if (!(reg & SFLASH_WRITE_IN_PROGRESS))
			return 0;
		if (stopwatch_expired(&sw))
			return -1;
	}

	return -1;
}

/* set serial flash program address */
static void set_sfpaddr(u32 addr)
{
	write8(&mt8173_nor->radr[2], get_nth_byte(addr, 2));
	write8(&mt8173_nor->radr[1], get_nth_byte(addr, 1));
	write8(&mt8173_nor->radr[0], get_nth_byte(addr, 0));
}

static int sector_erase(int offset)
{
	if (wait_for_write_done())
		return -1;

	write8(&mt8173_nor->prgdata[5], SFLASH_OP_WREN);
	write8(&mt8173_nor->cnt, 8);
	mt8173_nor_execute_cmd(SFLASH_PRG_CMD);

	write8(&mt8173_nor->prgdata[5], SECTOR_ERASE_CMD);
	write8(&mt8173_nor->prgdata[4], get_nth_byte(offset, 2));
	write8(&mt8173_nor->prgdata[3], get_nth_byte(offset, 1));
	write8(&mt8173_nor->prgdata[2], get_nth_byte(offset, 0));
	write8(&mt8173_nor->cnt, 32);
	mt8173_nor_execute_cmd(SFLASH_PRG_CMD);

	if (wait_for_write_done())
		return -1;

	return 0;
}

static int dma_read(u32 addr, u8 *buf, u32 len, uintptr_t dma_buf,
		    size_t dma_buf_len)
{
	struct stopwatch sw;

	assert(IS_ALIGNED((uintptr_t)buf, SFLASH_DMA_ALIGN) &&
	       IS_ALIGNED(len, SFLASH_DMA_ALIGN) &&
	       len <= dma_buf_len);

	/* do dma reset */
	write32(&mt8173_nor->fdma_ctl, SFLASH_DMA_SW_RESET);
	write32(&mt8173_nor->fdma_ctl, SFLASH_DMA_WDLE_EN);
	/* flash source address and dram dest address */
	write32(&mt8173_nor->fdma_fadr, addr);
	write32(&mt8173_nor->fdma_dadr, dma_buf);
	write32(&mt8173_nor->fdma_end_dadr, (dma_buf + len));
	/* start dma */
	write32(&mt8173_nor->fdma_ctl, SFLASH_DMA_TRIGGER | SFLASH_DMA_WDLE_EN);

	stopwatch_init_usecs_expire(&sw, SFLASH_POLLINGREG_US);
	while ((read32(&mt8173_nor->fdma_ctl) & SFLASH_DMA_TRIGGER) != 0) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_WARNING, "dma read timeout!\n");
			return -1;
		}
	}

	memcpy(buf, (const void *)dma_buf, len);
	return 0;
}

static int pio_read(u32 addr, u8 *buf, u32 len)
{
	set_sfpaddr(addr);
	while (len) {
		if (mt8173_nor_execute_cmd(SFLASH_RD_TRIGGER | SFLASH_AUTOINC))
			return -1;

		*buf++ = read8(&mt8173_nor->rdata);
		len--;
	}
	return 0;
}

static int nor_read(const struct spi_flash *flash, u32 addr, size_t len,
		void *buf)
{
	u32 next;

	size_t done = 0;
	uintptr_t dma_buf;
	size_t dma_buf_len;

	if (!IS_ALIGNED((uintptr_t)buf, SFLASH_DMA_ALIGN)) {
		next = MIN(ALIGN_UP((uintptr_t)buf, SFLASH_DMA_ALIGN) -
			   (uintptr_t)buf, len);
		if (pio_read(addr, buf, next))
			return -1;
		done += next;
	}

	if (ENV_BOOTBLOCK || ENV_VERSTAGE) {
		dma_buf = (uintptr_t)_dma_coherent;
		dma_buf_len = _dma_coherent_size;
	} else {
		dma_buf = (uintptr_t)_dram_dma;
		dma_buf_len = _dram_dma_size;
	}

	while (len - done >= SFLASH_DMA_ALIGN) {
		next = MIN(dma_buf_len, ALIGN_DOWN(len - done,
			   SFLASH_DMA_ALIGN));
		if (dma_read(addr + done, buf + done, next, dma_buf,
			     dma_buf_len))
			return -1;
		done += next;
	}
	next = len - done;
	if (next > 0 && pio_read(addr + done, buf + done, next))
		return -1;
	return 0;
}

static int nor_write(const struct spi_flash *flash, u32 addr, size_t len,
		const void *buf)
{
	const u8 *buffer = (const u8 *)buf;

	set_sfpaddr(addr);
	while (len) {
		write8(&mt8173_nor->wdata, *buffer);
		if (mt8173_nor_execute_cmd(SFLASH_WR_TRIGGER | SFLASH_AUTOINC))
			return -1;

		if (wait_for_write_done())
			return -1;
		buffer++;
		len--;
	}
	return 0;
}

static int nor_erase(const struct spi_flash *flash, u32 offset, size_t len)
{
	int sector_start = offset;
	int sector_num = (u32)len / flash->sector_size;

	while (sector_num) {
		if (!sector_erase(sector_start)) {
			sector_start += flash->sector_size;
			sector_num--;
		} else {
			printk(BIOS_WARNING, "Erase failed at 0x%x!\n",
			       sector_start);
			return -1;
		}
	}
	return 0;
}

const struct spi_flash_ops spi_flash_ops = {
	.read = nor_read,
	.write = nor_write,
	.erase = nor_erase,
};

int mtk_spi_flash_probe(const struct spi_slave *spi,
				struct spi_flash *flash)
{
	write32(&mt8173_nor->wrprot, SFLASH_COMMAND_ENABLE);
	memcpy(&flash->spi, spi, sizeof(*spi));

	flash->name = "mt8173 flash controller";
	flash->sector_size = 0x1000;
	flash->erase_cmd = SECTOR_ERASE_CMD;
	flash->size = CONFIG_ROM_SIZE;

	flash->ops = &spi_flash_ops;

	return 0;
}
