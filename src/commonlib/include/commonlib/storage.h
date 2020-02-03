/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef __COMMONLIB_STORAGE_H__
#define __COMMONLIB_STORAGE_H__

#include <commonlib/sd_mmc_ctrlr.h>

/*
 * EXT_CSD fields
 */
#define EXT_CSD_GP_SIZE_MULT_GP0	143	/* RO */
#define EXT_CSD_GP_SIZE_MULT_GP1	146	/* RO */
#define EXT_CSD_GP_SIZE_MULT_GP2	149	/* RO */
#define EXT_CSD_GP_SIZE_MULT_GP3	152	/* RO */
#define EXT_CSD_PARTITIONING_SUPPORT	160	/* RO */
#define EXT_CSD_RPMB_SIZE_MULT		168	/* RO */
#define EXT_CSD_ERASE_GROUP_DEF		175	/* R/W */
#define EXT_CSD_PART_CONF		179	/* R/W */
#define EXT_CSD_BUS_WIDTH		183	/* R/W */
#define EXT_CSD_STROBE_SUPPORT		184	/* RO */
#define EXT_CSD_HS_TIMING		185	/* R/W */
#define EXT_CSD_REV			192	/* RO */
#define EXT_CSD_CARD_TYPE		196	/* RO */
#define EXT_CSD_SEC_CNT			212	/* RO, 4 bytes */
#define EXT_CSD_HC_WP_GRP_SIZE		221	/* RO */
#define EXT_CSD_HC_ERASE_GRP_SIZE	224	/* RO */
#define EXT_CSD_BOOT_SIZE_MULT		226	/* RO */
#define EXT_CSD_TRIM_MULT		232     /* RO */

/*
 * EXT_CSD field definitions
 */

#define EXT_CSD_CMD_SET_NORMAL		(1 << 0)
#define EXT_CSD_CMD_SET_SECURE		(1 << 1)
#define EXT_CSD_CMD_SET_CPSECURE	(1 << 2)

#define EXT_CSD_CARD_TYPE_26	(1 << 0)	/* Card can run at 26MHz */
#define EXT_CSD_CARD_TYPE_52	(1 << 1)	/* Card can run at 52MHz */

#define EXT_CSD_BUS_WIDTH_1		0	/* Card is in 1 bit mode */
#define EXT_CSD_BUS_WIDTH_4		1	/* Card is in 4 bit mode */
#define EXT_CSD_BUS_WIDTH_8		2	/* Card is in 8 bit mode */
#define EXT_CSD_DDR_BUS_WIDTH_4		5	/* Card is in 4 bit DDR mode */
#define EXT_CSD_DDR_BUS_WIDTH_8		6	/* Card is in 8 bit DDR mode */
#define EXT_CSD_BUS_WIDTH_STROBE	(1<<7)	/* Enhanced strobe mode */

#define EXT_CSD_TIMING_BC		0	/* Backwards compatibility */
#define EXT_CSD_TIMING_HS		1	/* High speed */
#define EXT_CSD_TIMING_HS200		2	/* HS200 */
#define EXT_CSD_TIMING_HS400		3	/* HS400 */

#define EXT_CSD_SIZE			512

/* 179: EXT_CSD_PART_CONF */
#define EXT_CSD_PART_ACCESS_MASK	7	/* Partition access mask */

/* 175: EXT_CSD_ERASE_GROUP_DEF */
#define EXT_CSD_PARTITION_ENABLE	1	/* Enable partition access */

struct storage_media {
	uint64_t capacity[8];		/* Partition capacity in bytes */
	struct sd_mmc_ctrlr *ctrlr;

#define MMC_PARTITION_USER		0
#define MMC_PARTITION_BOOT_1		1
#define MMC_PARTITION_BOOT_2		2
#define MMC_PARTITION_RPMB		3
#define MMC_PARTITION_GP1		4
#define MMC_PARTITION_GP2		5
#define MMC_PARTITION_GP3		6
#define MMC_PARTITION_GP4		7

	uint32_t caps;
	uint32_t version;

#define SD_VERSION_SD		0x20000
#define SD_VERSION_2		(SD_VERSION_SD | 0x20)
#define SD_VERSION_1_0		(SD_VERSION_SD | 0x10)
#define SD_VERSION_1_10		(SD_VERSION_SD | 0x1a)
#define MMC_VERSION_MMC		0x10000
#define MMC_VERSION_UNKNOWN	(MMC_VERSION_MMC)
#define MMC_VERSION_1_2		(MMC_VERSION_MMC | 0x12)
#define MMC_VERSION_1_4		(MMC_VERSION_MMC | 0x14)
#define MMC_VERSION_2_2		(MMC_VERSION_MMC | 0x22)
#define MMC_VERSION_3		(MMC_VERSION_MMC | 0x30)
#define MMC_VERSION_4		(MMC_VERSION_MMC | 0x40)

	uint32_t read_bl_len;
	uint32_t write_bl_len;
	int high_capacity;
	uint32_t tran_speed;
	/* Erase size in terms of block length. */
	uint32_t erase_blocks;
	/* Trim operation multiplier for determining timeout. */
	uint32_t trim_mult;

	uint32_t ocr;

#define OCR_BUSY		0x80000000
#define OCR_HCS			0x40000000
#define OCR_VOLTAGE_MASK	0x00FFFF80
#define OCR_ACCESS_MODE		0x60000000

	uint32_t op_cond_response; // The response byte from the last op_cond

	uint32_t scr[2];
	uint32_t csd[4];
	uint32_t cid[4];
	uint16_t rca;

	uint8_t partition_config;	/* Duplicate of EXT_CSD_PART_CONF */
};

uint64_t storage_block_erase(struct storage_media *media, uint64_t start,
	uint64_t count);
uint64_t storage_block_fill_write(struct storage_media *media, uint64_t start,
	uint64_t count, uint32_t fill_pattern);
uint64_t storage_block_read(struct storage_media *media, uint64_t start,
	uint64_t count, void *buffer);
uint64_t storage_block_write(struct storage_media *media, uint64_t start,
	uint64_t count, const void *buffer);

unsigned int storage_get_current_partition(struct storage_media *media);
const char *storage_partition_name(struct storage_media *media,
	unsigned int partition_number);
int storage_setup_media(struct storage_media *media,
	struct sd_mmc_ctrlr *ctrlr);

int storage_set_partition(struct storage_media *media,
	unsigned int partition_number);

void storage_display_setup(struct storage_media *media);

#endif /* __COMMONLIB_STORAGE_H__ */
