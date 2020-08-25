/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_MT8192_FLASH_CONTROLLER_H__
#define __SOC_MEDIATEK_MT8192_FLASH_CONTROLLER_H__

#include <spi-generic.h>
#include <stdint.h>
#include <soc/addressmap.h>

enum {
	SFLASH_POLLINGREG_US	  = 500000,
	SFLASH_WRBUF_SIZE	  = 128,
	SFLASHNAME_LENGTH	  = 16,
	SFLASH_WRITE_IN_PROGRESS  = 1,
	SFLASH_COMMAND_ENABLE	  = 0x30,
	SFLASH_DMA_ALIGN	  = 0x10,

	/* NOR flash controller commands */
	SFLASH_RD_TRIGGER	  = 1 << 0,
	SFLASH_READSTATUS	  = 1 << 1,
	SFLASH_PRG_CMD		  = 1 << 2,
	SFLASH_WR_TRIGGER	  = 1 << 4,
	SFLASH_WRITESTATUS	  = 1 << 5,
	SFLASH_AUTOINC		  = 1 << 7,
	/* NOR flash commands */
	SFLASH_OP_WREN		  = 0x6,
	SECTOR_ERASE_CMD	  = 0x20,
	SFLASH_UNPROTECTED	  = 0x0,
	/* DMA commands */
	SFLASH_DMA_TRIGGER	  = 1 << 0,
	SFLASH_DMA_SW_RESET	  = 1 << 1,
	SFLASH_DMA_WDLE_EN	  = 1 << 2,
	/* Dual mode */
	SFLASH_READ_DUAL_EN	  = 0x1,
	SFLASH_1_1_2_READ	  = 0x3b
};

/* register Offset */
struct mt8192_nor_regs {
	u32 cmd;
	u32 cnt;
	u32 rdsr;
	u32 rdata;
	u32 radr[3];
	u32 wdata;
	u32 prgdata[6];
	u32 shreg[10];
	u32 cfg[2];
	u32 shreg10;
	u32 status[5];
	u32 timing;
	u32 flash_cfg;
	u32 reserved2[3];
	u32 sf_time;
	u32 reserved3;
	u32 diff_addr;
	u32 del_sel[2];
	u32 intrstus;
	u32 intren;
	u32 pp_ctl;
	u32 cfg3;
	u32 chksum_ctl;
	u32 chksum;
	u32 aaicmd;
	u32 wrprot;
	u32 radr3;
	u32 read_dual;
	u32 delsel[3];
	u32 reserved[397];
	u32 cfg1_bri[2];
	u32 fdma_ctl;
	u32 fdma_fadr;
	u32 fdma_dadr;
	u32 fdma_end_dadr;
};
check_member(mt8192_nor_regs, fdma_end_dadr, 0x724);
static struct mt8192_nor_regs *const mt8192_nor = (void *)SFLASH_REG_BASE;

int mtk_spi_flash_probe(const struct spi_slave *spi, struct spi_flash *flash);

#endif /* __SOC_MEDIATEK_MT8192_FLASH_CONTROLLER_H__ */
