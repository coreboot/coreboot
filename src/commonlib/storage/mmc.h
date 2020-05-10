/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __COMMONLIB_STORAGE_MMC_H__
#define __COMMONLIB_STORAGE_MMC_H__

#include <commonlib/sd_mmc_ctrlr.h>

#define MMC_HS_TIMING		0x00000100
#define MMC_HS_52MHZ		0x2
#define MMC_HS_200MHZ		0x10
#define MMC_HS400		0x40

#define SECURE_ERASE		0x80000000

#define MMC_STATUS_MASK		(~0x0206BF7F)
#define MMC_STATUS_RDY_FOR_DATA (1 << 8)
#define MMC_STATUS_CURR_STATE	(0xf << 9)
#define MMC_STATUS_ERROR	(1 << 19)

#define MMC_SWITCH_MODE_CMD_SET		0x00 /* Change the command set */
#define MMC_SWITCH_MODE_SET_BITS	0x01 /* Set bits in EXT_CSD byte
						addressed by index which are
						1 in value field */
#define MMC_SWITCH_MODE_CLEAR_BITS	0x02 /* Clear bits in EXT_CSD byte
						addressed by index, which are
						1 in value field */
#define MMC_SWITCH_MODE_WRITE_BYTE	0x03 /* Set target byte to value */

#define R1_ILLEGAL_COMMAND		(1 << 22)
#define R1_APP_CMD			(1 << 5)

#define MMC_INIT_TIMEOUT_US	(1000 * 1000)
#define MMC_INIT_TIMEOUT_US_MS	1000

int storage_block_setup_media(struct storage_media *media,
	struct sd_mmc_ctrlr *ctrlr);

#endif /* __COMMONLIB_STORAGE_MMC_H__ */
