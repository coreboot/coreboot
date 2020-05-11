/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <soc/flash_controller.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <stdint.h>
#include <string.h>
#include <timer.h>
#include <types.h>

#define GET_NTH_BYTE(d, n)	((d >> (8 * n)) & 0xff)

static int polling_cmd(u32 val)
{
	struct stopwatch sw;

	stopwatch_init_usecs_expire(&sw, SFLASH_POLLINGREG_US);

	while ((read32(&mt8192_nor->cmd) & val) != 0) {
		if (stopwatch_expired(&sw))
			return -1;
	}

	return 0;
}

static int mt8192_nor_execute_cmd(u8 cmdval)
{
	u8 val = cmdval & ~SFLASH_AUTOINC;

	write8(&mt8192_nor->cmd, cmdval);
	return polling_cmd(val);
}

static int sflashhw_read_flash_status(u8 *value)
{
	if (mt8192_nor_execute_cmd(SFLASH_READSTATUS))
		return -1;

	*value = read8(&mt8192_nor->rdsr);
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
	write8(&mt8192_nor->radr[2], GET_NTH_BYTE(addr, 2));
	write8(&mt8192_nor->radr[1], GET_NTH_BYTE(addr, 1));
	write8(&mt8192_nor->radr[0], GET_NTH_BYTE(addr, 0));
}

static int sector_erase(int offset)
{
	if (wait_for_write_done())
		return -1;

	write8(&mt8192_nor->prgdata[5], SFLASH_OP_WREN);
	write8(&mt8192_nor->cnt, 8);
	mt8192_nor_execute_cmd(SFLASH_PRG_CMD);

	write8(&mt8192_nor->prgdata[5], SECTOR_ERASE_CMD);
	write8(&mt8192_nor->prgdata[4], GET_NTH_BYTE(offset, 2));
	write8(&mt8192_nor->prgdata[3], GET_NTH_BYTE(offset, 1));
	write8(&mt8192_nor->prgdata[2], GET_NTH_BYTE(offset, 0));
	write8(&mt8192_nor->cnt, 32);
	mt8192_nor_execute_cmd(SFLASH_PRG_CMD);

	if (wait_for_write_done())
		return -1;

	return 0;
}

static int pio_read(u32 addr, u8 *buf, u32 len)
{
	set_sfpaddr(addr);
	while (len) {
		if (mt8192_nor_execute_cmd(SFLASH_RD_TRIGGER | SFLASH_AUTOINC))
			return -1;

		*buf++ = read8(&mt8192_nor->rdata);
		len--;
	}
	return 0;
}

static int nor_read(const struct spi_flash *flash, u32 addr, size_t len,
		    void *buf)
{
	if (pio_read(addr, buf, len))
		return -1;

	return 0;
}

static int nor_write(const struct spi_flash *flash, u32 addr, size_t len,
		     const void *buf)
{
	const u8 *buffer = (const u8 *)buf;

	set_sfpaddr(addr);
	while (len) {
		write8(&mt8192_nor->wdata, *buffer);
		if (mt8192_nor_execute_cmd(SFLASH_WR_TRIGGER | SFLASH_AUTOINC))
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
			printk(BIOS_WARNING, "Erase failed at %#x!\n",
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
	write32(&mt8192_nor->wrprot, SFLASH_COMMAND_ENABLE);
	memcpy(&flash->spi, spi, sizeof(*spi));

	flash->sector_size = 0x1000;
	flash->erase_cmd = SECTOR_ERASE_CMD;
	flash->size = CONFIG_ROM_SIZE;

	flash->ops = &spi_flash_ops;

	return 0;
}
