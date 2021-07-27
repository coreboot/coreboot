/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _COMMON_QUPV3_CONFIG_H_
#define _COMMON_QUPV3_CONFIG_H_

#include <assert.h>
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
#define SEFW_MAGIC_HEADER					0x57464553

#define GSI_FW_MAGIC_HEADER					0x20495351
#define GSI_REG_BASE_SIZE					0x5000
#define GSI_INST_RAM_n_MAX_n					4095
#define GSI_FW_BYTES_PER_LINE					8
#define GSI_MCS_CFG_MCS_ENABLE_BMSK				0x1
#define GSI_CFG_DOUBLE_MCS_CLK_FREQ_BMSK			0x4
#define GSI_CFG_GSI_ENABLE_BMSK					0x1
#define GSI_CGC_CTRL_REGION_2_HW_CGC_EN_BMSK			0x2


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

struct gsi_fw_hdr {
	uint32_t magic;		   /* = 'QSI' */
	uint32_t version;	   /* Structure version number */
	uint32_t core_version;	   /* QUPV3_HW_VERSION */
	uint32_t fw_version;	   /* Programmed into GSI_FW_REVISION */
	uint16_t fw_size_in_items; /* Number of GSI FW lines (each 8 bytes) */
	uint16_t fw_offset;	   /* Byte offset of GENI_FW_RAM array */
	uint16_t iep_size_in_items;/* Number of IEP items */
	uint16_t iep_offset;	   /* Byte offset of first IEP entry */
};

struct gsi_fw_iram {
	uint32_t iram_dword0;	/* word 0 of a single IRAM firmware entry */
	uint32_t iram_dword1;	/* word 1 of a single IRAM firmware entry */
};

struct gsi_fw_iep {
	uint32_t offset;	/* offset from QUPV3_GSI_TOP */
	uint32_t value ;	/* value to be written into above offset */
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
void gpi_firmware_load(int addr);

#endif /* _COMMON_QUPV3_CONFIG_H_ */
