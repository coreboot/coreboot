/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOC_MEDIATEK_COMMON_MSDC_H
#define SOC_MEDIATEK_COMMON_MSDC_H

#include <commonlib/sd_mmc_ctrlr.h>
#include <console/console.h>

/*------------------------------*/
/* Register Offset              */
/*------------------------------*/
#define MSDC_CFG	0x0
#define MSDC_IOCON	0x04
#define MSDC_PS		0x08
#define MSDC_INT	0x0c
#define MSDC_INTEN	0x10
#define MSDC_FIFOCS	0x14
#define MSDC_TXDATA	0x18
#define MSDC_RXDATA	0x1c
#define SDC_CFG		0x30
#define SDC_CMD		0x34
#define SDC_ARG		0x38
#define SDC_STS		0x3c
#define SDC_RESP0	0x40
#define SDC_RESP1	0x44
#define SDC_RESP2	0x48
#define SDC_RESP3	0x4c
#define SDC_BLK_NUM	0x50
#define SDC_ADV_CFG0	0x64
#define EMMC_IOCON	0x7c
#define SDC_ACMD_RESP	0x80
#define DMA_SA_H4BIT	0x8c
#define MSDC_DMA_SA	0x90
#define MSDC_DMA_CTRL	0x98
#define MSDC_DMA_CFG	0x9c
#define MSDC_PATCH_BIT	0xb0
#define MSDC_PATCH_BIT1	0xb4
#define MSDC_PATCH_BIT2	0xb8
#define MSDC_PAD_TUNE	0xec
#define MSDC_PAD_TUNE0	0xf0
#define PAD_DS_TUNE	0x188
#define PAD_CMD_TUNE	0x18c
#define EMMC51_CFG0	0x204
#define EMMC50_CFG0	0x208
#define EMMC50_CFG1	0x20c
#define EMMC50_CFG3	0x220
#define SDC_FIFO_CFG	0x228

/*-------------------------------*/
/* Top Pad Register Offset       */
/*-------------------------------*/
#define EMMC_TOP_CONTROL	0x00
#define EMMC_TOP_CMD		0x04
#define EMMC50_PAD_DS_TUNE	0x0c

/*--------------------------------------------------------------------------*/
/* Register Mask                                                            */
/*--------------------------------------------------------------------------*/

/* MSDC_CFG mask */
#define MSDC_CFG_MODE		(0x1 << 0)	/* RW */
#define MSDC_CFG_CKPDN		(0x1 << 1)	/* RW */
#define MSDC_CFG_RST		(0x1 << 2)	/* RW */
#define MSDC_CFG_PIO		(0x1 << 3)	/* RW */
#define MSDC_CFG_CKSTB		(0x1 << 7)	/* R  */

/* MSDC_IOCON mask */
#define MSDC_IOCON_DDLSEL	(0x1 << 3)	/* RW */

/* MSDC_INT mask */
#define MSDC_INT_CMDRDY		(0x1 << 8)	/* W1C */
#define MSDC_INT_CMDTMO		(0x1 << 9)	/* W1C */
#define MSDC_INT_RSPCRCERR	(0x1 << 10)	/* W1C */

/* MSDC_FIFOCS mask */
#define MSDC_FIFOCS_RXCNT	(0xff << 0)	/* R */
#define MSDC_FIFOCS_TXCNT	(0xff << 16)	/* R */
#define MSDC_FIFOCS_CLR		(0x1 << 31)	/* RW */

/* SDC_CFG mask */
#define SDC_CFG_BUSWIDTH	(0x3 << 16)	/* RW */
#define SDC_CFG_SDIO		(0x1 << 19)	/* RW */
#define SDC_CFG_SDIOIDE		(0x1 << 20)	/* RW */
#define SDC_CFG_DTOC		(0xff << 24)	/* RW */

/* SDC_CMD */
#define SDC_CMD_CMD_S		0
#define SDC_CMD_CMD_M		(0x3f << SDC_CMD_CMD_S)
#define SDC_CMD_RSPTYP_S	7
#define SDC_CMD_RSPTYP_M	(0x7 << SDC_CMD_RSPTYP_S)
#define SDC_CMD_DTYPE_S		11
#define SDC_CMD_DTYPE_M		(0x3 << SDC_CMD_DTYPE_S)
#define SDC_CMD_WR		(1 << 13)
#define SDC_CMD_STOP		(1 << 14)
#define SDC_CMD_BLK_LEN_S	16
#define SDC_CMD_BLK_LEN_M	(0xfff << SDC_CMD_BLK_LEN_S)

/* SDC_STS mask */
#define SDC_STS_SDCBUSY		(0x1 << 0)	/* RW */
#define SDC_STS_CMDBUSY		(0x1 << 1)	/* RW */

/* SDC_ADV_CFG0 mask */
#define SDC_DAT1_IRQ_TRIGGER	(0x1 << 19)	/* RW */
#define SDC_RX_ENHANCE_EN	(0x1 << 20)	/* RW */

/* MSDC_PATCH_BIT mask */
#define MSDC_CKGEN_MSDC_DLY_SEL	(0x1f << 10)

/* PATCH_BIT1 mask */
#define MSDC_PATCH_BIT1_STOP_DLY	(0xf << 8)	/* RW */

/* PATCH_BIT2 mask */
#define MSDC_PATCH_BIT2_CFGRESP		(0x1 << 15)	/* RW */
#define MSDC_PATCH_BIT2_CFGCRCSTS	(0x1 << 28)	/* RW */
#define MSDC_PB2_RESPWAIT		(0x3 << 2)	/* RW */

/* PAD_TUNE mask */
#define MSDC_PAD_TUNE_RD_SEL	(0x1 << 13)	/* RW */
#define MSDC_PAD_TUNE_CMD_SEL	(0x1 << 21)	/* RW */

/* EMMC50_CFG mask */
#define EMMC50_CFG_CFCSTS_SEL	(0x1 << 4)	/* RW */

/* SDC_FIFO mask */
#define SDC_FIFO_CFG_WRVALIDSEL	(0x1 << 24)	/* RW */
#define SDC_FIFO_CFG_RDVALIDSEL	(0x1 << 25)	/* RW */

/* EMMC_TOP_CONTROL mask */
#define PAD_DAT_RD_RXDLY_SEL	(0x1 << 13)	/* RW */
#define DATA_K_VALUE_SEL	(0x1 << 14)	/* RW */
#define SDC_RX_ENH_EN		(0x1 << 15)	/* TW */

/* EMMC_TOP_CMD mask */
#define PAD_CMD_RD_RXDLY_SEL	(0x1 << 11)	/* RW */

#define CMD_TIMEOUT_MS		(5 * 100)	/* 500ms */
#define MSDC_TIMEOUT_US		(1000 * 1000)	/* 1s */

/* SDC_CFG_BUSWIDTH */
#define MSDC_BUS_1BITS	0x0
#define MSDC_BUS_4BITS	0x1
#define MSDC_BUS_8BITS	0x2

#define MSDC_SUCCESS	0x0
#define MSDC_NOT_READY	0x1

#define EIO		5	/* I/O error */
#define ETIMEDOUT	110	/* I/O timed out */

#define CMD_INTS_MASK   \
	(MSDC_INT_CMDRDY | MSDC_INT_RSPCRCERR | MSDC_INT_CMDTMO)

struct msdc_ctrlr {
	struct sd_mmc_ctrlr sd_mmc_ctrlr;
	void *base;		/* IO address */
	void *top_base;		/* Top IO address */

	uint32_t clock;		/* Current clock frequency */
	uint32_t src_hz;	/* Source clock frequency */

	bool initialized;
};

#define msdc_debug(format...) printk(BIOS_DEBUG, format)
#define msdc_trace(format...) printk(BIOS_DEBUG, format)
#define msdc_error(format...) printk(BIOS_ERR, format)

int mtk_emmc_early_init(void *base, void *top_base);
void mtk_msdc_configure_emmc(bool is_early_init);
void mtk_msdc_configure_sdcard(void);

#endif /* SOC_MEDIATEK_COMMON_MSDC_H */
