/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Controller independent definitions
 */
#ifndef __COMMONLIB_SD_MMC_CTRLR_H__
#define __COMMONLIB_SD_MMC_CTRLR_H__

#include <stdint.h>

/* Error values returned by the storage drivers */
#define CARD_UNUSABLE_ERR	-17 /* Unusable Card */
#define CARD_COMM_ERR		-18 /* Communications Error */
#define CARD_TIMEOUT		-19
#define CARD_IN_PROGRESS	-20 /* operation is in progress */

/* MMC status in CBMEM_ID_MMC_STATUS */
enum {
	MMC_STATUS_NEED_RESET = 0,
	MMC_STATUS_CMD1_READY_OR_IN_PROGRESS,
	MMC_STATUS_CMD1_READY,		/* Byte mode */
	MMC_STATUS_CMD1_IN_PROGRESS,
	MMC_STATUS_CMD1_READY_HCS,	/* Sector mode (High capacity support) */
};

struct mmc_command {
	uint16_t cmdidx;

/* Common commands */
#define MMC_CMD_GO_IDLE_STATE		0
#define MMC_CMD_SEND_OP_COND		1
#define MMC_CMD_ALL_SEND_CID		2
#define MMC_CMD_SET_DSR			4
#define MMC_CMD_SELECT_CARD		7
#define MMC_CMD_SEND_CSD		9
#define MMC_CMD_SEND_CID		10
#define MMC_CMD_STOP_TRANSMISSION	12
#define MMC_CMD_SEND_STATUS		13
#define MMC_CMD_SET_BLOCKLEN		16
#define MMC_CMD_READ_SINGLE_BLOCK	17
#define MMC_CMD_READ_MULTIPLE_BLOCK	18
#define MMC_CMD_WRITE_SINGLE_BLOCK	24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK	25
#define MMC_CMD_APP_CMD			55

/* MMC specific commands */
#define MMC_CMD_SET_RELATIVE_ADDR	3
#define MMC_CMD_SWITCH			6
#define MMC_CMD_SEND_EXT_CSD		8
#define MMC_CMD_AUTO_TUNING_SEQUENCE	21
#define MMC_CMD_ERASE_GROUP_START	35
#define MMC_CMD_ERASE_GROUP_END		36
#define MMC_CMD_ERASE			38
#define MMC_CMD_SPI_READ_OCR		58
#define MMC_CMD_SPI_CRC_ON_OFF		59

/* SD specific commands */
#define SD_CMD_SEND_RELATIVE_ADDR	3
#define SD_CMD_SWITCH_FUNC		6
#define SD_CMD_SEND_IF_COND		8
#define SD_CMD_ERASE_WR_BLK_START	32
#define SD_CMD_ERASE_WR_BLK_END		33

/* SD specific APP commands */
#define SD_CMD_APP_SET_BUS_WIDTH	6
#define SD_CMD_APP_SEND_OP_COND		41
#define SD_CMD_APP_SEND_SCR		51

	uint32_t resp_type;

#define CARD_RSP_PRESENT (1 << 0)
#define CARD_RSP_136	(1 << 1)		/* 136 bit response */
#define CARD_RSP_CRC	(1 << 2)		/* expect valid crc */
#define CARD_RSP_BUSY	(1 << 3)		/* card may send busy */
#define CARD_RSP_OPCODE	(1 << 4)		/* response contains opcode */

#define CARD_RSP_NONE	(0)
#define CARD_RSP_R1	(CARD_RSP_PRESENT|CARD_RSP_CRC|CARD_RSP_OPCODE)
#define CARD_RSP_R1b	(CARD_RSP_PRESENT|CARD_RSP_CRC|CARD_RSP_OPCODE| \
			CARD_RSP_BUSY)
#define CARD_RSP_R2	(CARD_RSP_PRESENT|CARD_RSP_136|CARD_RSP_CRC)
#define CARD_RSP_R3	(CARD_RSP_PRESENT)
#define CARD_RSP_R4	(CARD_RSP_PRESENT)
#define CARD_RSP_R5	(CARD_RSP_PRESENT|CARD_RSP_CRC|CARD_RSP_OPCODE)
#define CARD_RSP_R6	(CARD_RSP_PRESENT|CARD_RSP_CRC|CARD_RSP_OPCODE)
#define CARD_RSP_R7	(CARD_RSP_PRESENT|CARD_RSP_CRC|CARD_RSP_OPCODE)

	uint32_t cmdarg;

#define MMC_TRIM_ARG			0x1
#define MMC_SECURE_ERASE_ARG		0x80000000

	uint32_t response[4];
	uint32_t flags;

#define CMD_FLAG_IGNORE_INHIBIT	1
};

#define SD_SWITCH_CHECK		0
#define SD_SWITCH_SWITCH	1

#define SD_DATA_4BIT		0x00040000

/* SCR definitions in different words */
#define SD_HIGHSPEED_BUSY	0x00020000
#define SD_HIGHSPEED_SUPPORTED	0x00020000

struct mmc_data {
	union {
		char *dest;
		const char *src;
	};
	uint32_t flags;

#define DATA_FLAG_READ		1
#define DATA_FLAG_WRITE		2

	uint32_t blocks;
	uint32_t blocksize;
};

struct sd_mmc_ctrlr {
	int (*send_cmd)(struct sd_mmc_ctrlr *ctrlr,
		struct mmc_command *cmd, struct mmc_data *data);
	void (*set_ios)(struct sd_mmc_ctrlr *ctrlr);
	void (*tuning_start)(struct sd_mmc_ctrlr *ctrlr, int retune);
	int (*is_tuning_complete)(struct sd_mmc_ctrlr *ctrlr, int *successful);

	int initialized;
	unsigned int version;
	uint32_t voltages;

#define MMC_VDD_165_195		0x00000080	/* VDD voltage 1.65 - 1.95 */
#define MMC_VDD_20_21		0x00000100	/* VDD voltage 2.0 ~ 2.1 */
#define MMC_VDD_21_22		0x00000200	/* VDD voltage 2.1 ~ 2.2 */
#define MMC_VDD_22_23		0x00000400	/* VDD voltage 2.2 ~ 2.3 */
#define MMC_VDD_23_24		0x00000800	/* VDD voltage 2.3 ~ 2.4 */
#define MMC_VDD_24_25		0x00001000	/* VDD voltage 2.4 ~ 2.5 */
#define MMC_VDD_25_26		0x00002000	/* VDD voltage 2.5 ~ 2.6 */
#define MMC_VDD_26_27		0x00004000	/* VDD voltage 2.6 ~ 2.7 */
#define MMC_VDD_27_28		0x00008000	/* VDD voltage 2.7 ~ 2.8 */
#define MMC_VDD_28_29		0x00010000	/* VDD voltage 2.8 ~ 2.9 */
#define MMC_VDD_29_30		0x00020000	/* VDD voltage 2.9 ~ 3.0 */
#define MMC_VDD_30_31		0x00040000	/* VDD voltage 3.0 ~ 3.1 */
#define MMC_VDD_31_32		0x00080000	/* VDD voltage 3.1 ~ 3.2 */
#define MMC_VDD_32_33		0x00100000	/* VDD voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34		0x00200000	/* VDD voltage 3.3 ~ 3.4 */
#define MMC_VDD_34_35		0x00400000	/* VDD voltage 3.4 ~ 3.5 */
#define MMC_VDD_35_36		0x00800000	/* VDD voltage 3.5 ~ 3.6 */

#define MMC_VDD_165_195_SHIFT   7

	uint32_t clock_base;	/* Controller's base clock */
	uint32_t f_min;
	uint32_t f_max;
	uint32_t request_hz;	/* Desired clock frequency */
	uint32_t bus_hz;	/* Actual bus clock frequency */

#define CLOCK_KHZ		1000
#define CLOCK_MHZ		(1000 * CLOCK_KHZ)
#define CLOCK_20MHZ		(20 * CLOCK_MHZ)
#define CLOCK_25MHZ		(25 * CLOCK_MHZ)
#define CLOCK_26MHZ		(26 * CLOCK_MHZ)
#define CLOCK_50MHZ		(50 * CLOCK_MHZ)
#define CLOCK_52MHZ		(52 * CLOCK_MHZ)
#define CLOCK_200MHZ		(200 * CLOCK_MHZ)

	uint32_t bus_width;
	uint32_t caps;

/* Generic controller & driver capabilities.  Controller specific capabilities
 * start at 0x00010000
 */
#define DRVR_CAP_4BIT				0x00000001
#define DRVR_CAP_8BIT				0x00000002
#define DRVR_CAP_AUTO_CMD12			0x00000004
#define DRVR_CAP_HC				0x00000008
#define DRVR_CAP_HS				0x00000010
#define DRVR_CAP_HS52				0x00000020
#define DRVR_CAP_HS200				0x00000040
#define DRVR_CAP_HS400				0x00000080
#define DRVR_CAP_ENHANCED_STROBE		0x00000100
#define DRVR_CAP_REMOVABLE			0x00000200
#define DRVR_CAP_DMA_64BIT			0x00000400
#define DRVR_CAP_HS200_TUNING			0x00000800

	uint32_t b_max;
	uint32_t timing;

#define BUS_TIMING_LEGACY	0
#define BUS_TIMING_MMC_HS	1
#define BUS_TIMING_SD_HS	2
#define BUS_TIMING_UHS_SDR12	3
#define BUS_TIMING_UHS_SDR25	4
#define BUS_TIMING_UHS_SDR50	5
#define BUS_TIMING_UHS_SDR104	6
#define BUS_TIMING_UHS_DDR50	7
#define BUS_TIMING_MMC_DDR52	8
#define BUS_TIMING_MMC_HS200	9
#define BUS_TIMING_MMC_HS400	10
#define BUS_TIMING_MMC_HS400ES	11

	uint32_t mdelay_before_cmd0;
	uint32_t mdelay_after_cmd0;
	uint32_t udelay_wait_after_cmd;
};

/* SOC specific routine to override ctrlr->caps and .voltages
 *
 * Set/clear the necessary DRVR_CAP_xxx bits in ctrlr->caps to specify the
 * controllers capabilities and driver workarounds.
 *
 * Set/clear the necessary MMC_VDD_xxx bits in ctrlr->voltages to specify the
 * controllers power support.
 */
void soc_sd_mmc_controller_quirks(struct sd_mmc_ctrlr *ctrlr);

/* Optional routines to support logging */
void sdhc_log_command(struct mmc_command *cmd);
void sdhc_log_command_issued(void);
void sdhc_log_response(uint32_t entries, uint32_t *response);
void sdhc_log_ret(int ret);

#endif /* __COMMONLIB_SD_MMC_CTRLR_H__ */
