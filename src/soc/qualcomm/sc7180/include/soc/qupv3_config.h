/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019, The Linux Foundation.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SC7180_QUPV3_CONFIG_H_
#define _SC7180_QUPV3_CONFIG_H_

#include <assert.h>
#include <cbfs.h>
#include <soc/clock.h>
#include <soc/qcom_qup_se.h>

#define QUPV3_COMMON_CFG_FAST_SWITCH_TO_HIGH_DISABLE_BMSK	0x00000001
#define QUPV3_SE_AHB_M_CFG_AHB_M_CLK_CGC_ON_BMSK		0x00000001

#define GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_BMSK			0x00000200
#define GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_BMSK			0x00000100

#define GENI_DMA_MODE_EN_GENI_DMA_MODE_EN_BMSK			0x00000001

#define DMA_TX_IRQ_EN_SET_RESET_DONE_EN_SET_BMSK		0x00000008
#define DMA_TX_IRQ_EN_SET_SBE_EN_SET_BMSK			0x00000004
#define DMA_TX_IRQ_EN_SET_DMA_DONE_EN_SET_BMSK			0x00000001

#define DMA_RX_IRQ_EN_SET_FLUSH_DONE_EN_SET_BMSK		0x00000010
#define DMA_RX_IRQ_EN_SET_RESET_DONE_EN_SET_BMSK		0x00000008
#define DMA_RX_IRQ_EN_SET_SBE_EN_SET_BMSK			0x00000004
#define DMA_RX_IRQ_EN_SET_DMA_DONE_EN_SET_BMSK			0x00000001

#define DMA_GENERAL_CFG_AHB_SEC_SLV_CLK_CGC_ON_BMSK		0x00000008
#define DMA_GENERAL_CFG_DMA_AHB_SLV_CLK_CGC_ON_BMSK		0x00000004
#define DMA_GENERAL_CFG_DMA_TX_CLK_CGC_ON_BMSK			0x00000002
#define DMA_GENERAL_CFG_DMA_RX_CLK_CGC_ON_BMSK			0x00000001

#define GENI_CLK_CTRL_SER_CLK_SEL_BMSK				0x00000001
#define DMA_IF_EN_DMA_IF_EN_BMSK				0x00000001
#define SE_GSI_EVENT_EN_BMSK					0x0000000f
#define SE_IRQ_EN_RMSK						0x0000000f

#define SIZE_GENI_FW_RAM					0x00000200
#define MAX_OFFSET_CFG_REG					0x000001c0
#define SEFW_MAGIC_HEADER					0x57464553

struct elf_se_hdr {
	uint32_t magic;		   /* = 'SEFW' */
	uint32_t version;	   /* Structure version number */
	uint32_t core_version;	   /* QUPV3_HW_VERSION */
	uint16_t serial_protocol;  /* Programmed into GENI_FW_REVISION */
	uint16_t fw_version;	   /* Programmed into GENI_FW_REVISION */
	uint16_t cfg_version;	   /* Programmed into GENI_INIT_CFG_REVISION */
	uint16_t fw_size_in_items; /* Number of (uint32_t) GENI_FW_RAM words */
	uint16_t fw_offset;	   /* Byte offset of GENI_FW_RAM array */
	uint16_t cfg_size_in_items;/* Number of GENI_FW_CFG index/value pairs */
	uint16_t cfg_idx_offset;   /* Byte offset of GENI_FW_CFG index array */
	uint16_t cfg_val_offset;   /* Byte offset of GENI_FW_CFG values array */
};

struct qupv3_common_reg {
	u8 reserved_1[0x118];
	u32 qupv3_se_ahb_m_cfg_reg;
	u8 reserved_2[0x4];
	u32 qupv3_common_cfg_reg;
};

void qupv3_fw_init(void);
void qupv3_se_fw_load_and_init(unsigned int bus, unsigned int protocol,
				unsigned int mode);

#endif /* _SC7180_QUPV3_CONFIG_H_ */
