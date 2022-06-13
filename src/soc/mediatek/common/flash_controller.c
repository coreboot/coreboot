/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <soc/flash_controller_common.h>
#include <soc/symbols.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <string.h>
#include <symbols.h>
#include <timer.h>
#include <types.h>

static struct mtk_nor_regs *const mtk_nor = (void *)SFLASH_REG_BASE;

#define GET_NTH_BYTE(d, n)	((d >> (8 * n)) & 0xff)

static int polling_cmd(u32 val)
{
	struct stopwatch sw;

	stopwatch_init_usecs_expire(&sw, SFLASH_POLLINGREG_US);

	while ((read32(&mtk_nor->cmd) & val) != 0) {
		if (stopwatch_expired(&sw))
			return -1;
	}

	return 0;
}

static int mtk_nor_execute_cmd(u8 cmdval)
{
	u8 val = cmdval & ~SFLASH_AUTOINC;

	write8(&mtk_nor->cmd, cmdval);
	return polling_cmd(val);
}

static int sflashhw_read_flash_status(u8 *value)
{
	if (mtk_nor_execute_cmd(SFLASH_READSTATUS))
		return -1;

	*value = read8(&mtk_nor->rdsr);
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
	write8(&mtk_nor->radr[2], GET_NTH_BYTE(addr, 2));
	write8(&mtk_nor->radr[1], GET_NTH_BYTE(addr, 1));
	write8(&mtk_nor->radr[0], GET_NTH_BYTE(addr, 0));
}

static int sector_erase(int offset)
{
	if (wait_for_write_done())
		return -1;

	write8(&mtk_nor->prgdata[5], SFLASH_OP_WREN);
	write8(&mtk_nor->cnt, 8);
	mtk_nor_execute_cmd(SFLASH_PRG_CMD);

	write8(&mtk_nor->prgdata[5], SECTOR_ERASE_CMD);
	write8(&mtk_nor->prgdata[4], GET_NTH_BYTE(offset, 2));
	write8(&mtk_nor->prgdata[3], GET_NTH_BYTE(offset, 1));
	write8(&mtk_nor->prgdata[2], GET_NTH_BYTE(offset, 0));
	write8(&mtk_nor->cnt, 32);
	mtk_nor_execute_cmd(SFLASH_PRG_CMD);

	if (wait_for_write_done())
		return -1;

	return 0;
}

static int dma_read(u32 addr, uintptr_t dma_buf, u32 len)
{
	struct stopwatch sw;

	assert(IS_ALIGNED((uintptr_t)addr, SFLASH_DMA_ALIGN) &&
	       IS_ALIGNED(len, SFLASH_DMA_ALIGN));

	/* do dma reset */
	write32(&mtk_nor->fdma_ctl, SFLASH_DMA_SW_RESET);
	write32(&mtk_nor->fdma_ctl, SFLASH_DMA_WDLE_EN);
	/* flash source address and dram dest address */
	write32(&mtk_nor->fdma_fadr, addr);
	write32(&mtk_nor->fdma_dadr, dma_buf);
	write32(&mtk_nor->fdma_end_dadr, (dma_buf + len));
	/* start dma */
	write32(&mtk_nor->fdma_ctl, SFLASH_DMA_TRIGGER | SFLASH_DMA_WDLE_EN);

	stopwatch_init_usecs_expire(&sw, SFLASH_POLLINGREG_US);
	while ((read32(&mtk_nor->fdma_ctl) & SFLASH_DMA_TRIGGER) != 0) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_WARNING, "dma read timeout!\n");
			return -1;
		}
	}

	return 0;
}

static int nor_read(const struct spi_flash *flash, u32 addr, size_t len,
		    void *buf)
{
	uintptr_t dma_buf = (uintptr_t)_dma_coherent;
	size_t dma_buf_len = REGION_SIZE(dma_coherent);
	u32 start = ALIGN_DOWN(addr, SFLASH_DMA_ALIGN);
	u32 skip = addr - start;
	u32 total = ALIGN_UP(skip + len, SFLASH_DMA_ALIGN);
	u32 drop = total - skip - len;
	u32 done, read_len, copy_len;
	uint8_t *dest = (uint8_t *)buf;

	/* Refer to CB:13989 for the hardware limitation on mt8173. */
	if (CONFIG(SOC_MEDIATEK_MT8173)) {
		if (!ENV_BOOTBLOCK && !ENV_SEPARATE_VERSTAGE) {
			dma_buf = (uintptr_t)_dram_dma;
			dma_buf_len = REGION_SIZE(dram_dma);
		}
	}

	if (CONFIG(FLASH_DUAL_IO_READ)) {
		setbits8(&mtk_nor->read_dual, SFLASH_READ_DUAL_EN);
		write8(&mtk_nor->prgdata[3], SFLASH_1_1_2_READ);
	}

	/* DMA: start [ skip | len | drop ] = total end */
	for (done = 0; done < total; dest += copy_len) {
		read_len = MIN(dma_buf_len, total - done);
		if (dma_read(start + done, dma_buf, read_len))
			return -1;

		done += read_len;
		/* decide the range to copy into buffer */
		if (done == total)
			read_len -= drop;  /* Only drop in last iteration */

		copy_len = read_len - skip;
		memcpy(dest, (uint8_t *)dma_buf + skip, copy_len);
		if (skip)
			skip = 0;  /* Only apply skip in first iteration. */
	}
	return 0;
}

static int nor_write(const struct spi_flash *flash, u32 addr, size_t len,
		     const void *buf)
{
	const u8 *buffer = (const u8 *)buf;

	set_sfpaddr(addr);
	while (len) {
		write8(&mtk_nor->wdata, *buffer);
		if (mtk_nor_execute_cmd(SFLASH_WR_TRIGGER | SFLASH_AUTOINC))
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
	write32(&mtk_nor->wrprot, SFLASH_COMMAND_ENABLE);
	memcpy(&flash->spi, spi, sizeof(*spi));

	flash->sector_size = 0x1000;
	flash->erase_cmd = SECTOR_ERASE_CMD;
	flash->size = CONFIG_ROM_SIZE;

	flash->ops = &spi_flash_ops;

	return 0;
}
