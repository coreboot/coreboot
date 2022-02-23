/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_COMMON_QCOM_QUP_SE_H__
#define __SOC_COMMON_QCOM_QUP_SE_H__

#include <console/console.h>
#include <device/mmio.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <timer.h>
#include <types.h>

#define QC_GENMASK(h, l)	(BIT(h + 1) - BIT(l))

/* GENI_OUTPUT_CTRL fields */
#define DEFAULT_IO_OUTPUT_CTRL_MSK	QC_GENMASK(6, 0)

/* GENI_FORCE_DEFAULT_REG fields */
#define FORCE_DEFAULT	BIT(0)

#define GENI_FW_REVISION_RO_PROTOCOL_MASK	0x0000FF00
#define GENI_FW_REVISION_RO_PROTOCOL_SHIFT	0x00000008

/* GENI_CGC_CTRL fields */
#define CFG_AHB_CLK_CGC_ON		BIT(0)
#define CFG_AHB_WR_ACLK_CGC_ON		BIT(1)
#define DATA_AHB_CLK_CGC_ON		BIT(2)
#define SCLK_CGC_ON			BIT(3)
#define TX_CLK_CGC_ON			BIT(4)
#define RX_CLK_CGC_ON			BIT(5)
#define EXT_CLK_CGC_ON			BIT(6)
#define PROG_RAM_HCLK_OFF		BIT(8)
#define PROG_RAM_SCLK_OFF		BIT(9)
#define DEFAULT_CGC_EN		(CFG_AHB_CLK_CGC_ON | CFG_AHB_WR_ACLK_CGC_ON \
			| DATA_AHB_CLK_CGC_ON | SCLK_CGC_ON \
			| TX_CLK_CGC_ON | RX_CLK_CGC_ON | EXT_CLK_CGC_ON)

/* GENI_SER_M_CLK_CFG/GENI_SER_S_CLK_CFG */
#define SER_CLK_EN			BIT(0)
#define CLK_DIV_SHFT			4
#define CLK_DIV_MSK			(0xFFF << CLK_DIV_SHFT)

/* FIFO_IF_DISABLE_RO fields */
#define FIFO_IF_DISABLE			BIT(0)

/* FW_REVISION_RO fields */
#define FW_REV_PROTOCOL_MSK	QC_GENMASK(15, 8)
#define FW_REV_PROTOCOL_SHFT	8
#define FW_REV_VERSION_SHFT		0

/* GENI_CLK_SEL fields */
#define CLK_SEL_MSK		QC_GENMASK(2, 0)

/* SE_GENI_DMA_MODE_EN */
#define GENI_DMA_MODE_EN	BIT(0)

/* GENI_M_CMD0 fields */
#define M_OPCODE_MSK		QC_GENMASK(31, 27)
#define M_OPCODE_SHFT		27
#define M_PARAMS_MSK		QC_GENMASK(26, 0)

/* GENI_M_CMD_CTRL_REG */
#define M_GENI_CMD_CANCEL	BIT(2)
#define M_GENI_CMD_ABORT	BIT(1)
#define M_GENI_DISABLE		BIT(0)

/* GENI_S_CMD0 fields */
#define S_OPCODE_MSK		QC_GENMASK(31, 27)
#define S_OPCODE_SHFT		27
#define S_PARAMS_MSK		QC_GENMASK(26, 0)

/* GENI_S_CMD_CTRL_REG */
#define S_GENI_CMD_CANCEL	BIT(2)
#define S_GENI_CMD_ABORT	BIT(1)
#define S_GENI_DISABLE		BIT(0)

/* GENI_M_IRQ_EN fields */
#define M_CMD_DONE_EN		BIT(0)
#define M_CMD_OVERRUN_EN	BIT(1)
#define M_ILLEGAL_CMD_EN	BIT(2)
#define M_CMD_FAILURE_EN	BIT(3)
#define M_CMD_CANCEL_EN		BIT(4)
#define M_CMD_ABORT_EN		BIT(5)
#define M_TIMESTAMP_EN		BIT(6)
#define M_RX_IRQ_EN		BIT(7)
#define M_GP_SYNC_IRQ_0_EN	BIT(8)
#define M_GP_IRQ_0_EN		BIT(9)
#define M_GP_IRQ_1_EN		BIT(10)
#define M_GP_IRQ_2_EN		BIT(11)
#define M_GP_IRQ_3_EN		BIT(12)
#define M_GP_IRQ_4_EN		BIT(13)
#define M_GP_IRQ_5_EN		BIT(14)
#define M_IO_DATA_DEASSERT_EN	BIT(22)
#define M_IO_DATA_ASSERT_EN	BIT(23)
#define M_RX_FIFO_RD_ERR_EN	BIT(24)
#define M_RX_FIFO_WR_ERR_EN	BIT(25)
#define M_RX_FIFO_WATERMARK_EN	BIT(26)
#define M_RX_FIFO_LAST_EN	BIT(27)
#define M_TX_FIFO_RD_ERR_EN	BIT(28)
#define M_TX_FIFO_WR_ERR_EN	BIT(29)
#define M_TX_FIFO_WATERMARK_EN	BIT(30)
#define M_SEC_IRQ_EN		BIT(31)
#define M_COMMON_GENI_M_IRQ_EN	(QC_GENMASK(6, 1) | \
				M_IO_DATA_DEASSERT_EN | \
				M_IO_DATA_ASSERT_EN | M_RX_FIFO_RD_ERR_EN | \
				M_RX_FIFO_WR_ERR_EN | M_TX_FIFO_RD_ERR_EN | \
				M_TX_FIFO_WR_ERR_EN)

/* GENI_S_IRQ_EN fields */
#define S_CMD_DONE_EN		BIT(0)
#define S_CMD_OVERRUN_EN	BIT(1)
#define S_ILLEGAL_CMD_EN	BIT(2)
#define S_CMD_FAILURE_EN	BIT(3)
#define S_CMD_CANCEL_EN		BIT(4)
#define S_CMD_ABORT_EN		BIT(5)
#define S_GP_SYNC_IRQ_0_EN	BIT(8)
#define S_GP_IRQ_0_EN		BIT(9)
#define S_GP_IRQ_1_EN		BIT(10)
#define S_GP_IRQ_2_EN		BIT(11)
#define S_GP_IRQ_3_EN		BIT(12)
#define S_GP_IRQ_4_EN		BIT(13)
#define S_GP_IRQ_5_EN		BIT(14)
#define S_IO_DATA_DEASSERT_EN	BIT(22)
#define S_IO_DATA_ASSERT_EN	BIT(23)
#define S_RX_FIFO_RD_ERR_EN	BIT(24)
#define S_RX_FIFO_WR_ERR_EN	BIT(25)
#define S_RX_FIFO_WATERMARK_EN	BIT(26)
#define S_RX_FIFO_LAST_EN	BIT(27)
#define S_COMMON_GENI_S_IRQ_EN	(QC_GENMASK(5, 1) | QC_GENMASK(13, 9) | \
				 S_RX_FIFO_RD_ERR_EN | S_RX_FIFO_WR_ERR_EN)

/*  GENI_/TX/RX/RX_RFR/_WATERMARK_REG fields */
#define WATERMARK_MSK		QC_GENMASK(5, 0)

/* GENI_TX_FIFO_STATUS fields */
#define TX_FIFO_WC		QC_GENMASK(27, 0)

/*  GENI_RX_FIFO_STATUS fields */
#define RX_LAST			BIT(31)
#define RX_LAST_BYTE_VALID_MSK	QC_GENMASK(30, 28)
#define RX_LAST_BYTE_VALID_SHFT	28
#define RX_FIFO_WC_MSK		QC_GENMASK(24, 0)

/* SE_IRQ_EN fields */
#define DMA_RX_IRQ_EN		BIT(0)
#define DMA_TX_IRQ_EN		BIT(1)
#define GENI_M_IRQ_EN		BIT(2)
#define GENI_S_IRQ_EN		BIT(3)

/* SE_DMA_GENERAL_CFG */
#define DMA_RX_CLK_CGC_ON	BIT(0)
#define DMA_TX_CLK_CGC_ON	BIT(1)
#define DMA_AHB_SLV_CFG_ON	BIT(2)
#define AHB_SEC_SLV_CLK_CGC_ON	BIT(3)
#define DUMMY_RX_NON_BUFFERABLE	BIT(4)
#define RX_DMA_ZERO_PADDING_EN	BIT(5)
#define RX_DMA_IRQ_DELAY_MSK	QC_GENMASK(8, 6)
#define RX_DMA_IRQ_DELAY_SHFT	6

#define DEFAULT_SE_CLK		(19200 * KHz)
#define GENI_DFS_IF_CFG_DFS_IF_EN_BMSK			BIT(0)

/* FIFO BUFFER PARAMETERS */
#define BYTES_PER_FIFO_WORD	4
#define FIFO_WIDTH		32
#define FIFO_DEPTH		16
#define BITS_PER_WORD	8
#define TX_WATERMARK	1

/* PACKING CONFIGURATION VECTOR */

/* start_idx:x: Bit position to move
 * direction:1: MSB to LSB
 * len:7: Represents bits-per-word = 8
 * stop:0: Till it's 1, FIFO bit shift continues
 */

/* Start_idx:7, direction:1, len:7, stop:0 */
#define PACK_VECTOR0	0x0FE
/* Start_idx:15, direction:1, len:7, stop:0 */
#define PACK_VECTOR1	0x1FE
/* Start_idx:23, direction:1, len:7, stop:0 */
#define PACK_VECTOR2	0x2FE
/* Start_idx:31, direction:1, len:7, stop:1 */
#define PACK_VECTOR3	0x3FF

enum se_protocol {
	SE_PROTOCOL_SPI  = 1,
	SE_PROTOCOL_UART = 2,
	SE_PROTOCOL_I2C  = 3,
	SE_PROTOCOL_I3C  = 4,
	SE_PROTOCOL_MAX  = 5
};

enum se_mode {
	NONE,
	GSI,
	FIFO,
	CPU_DMA,
	MIXED
};

struct qup_regs {
	u32 geni_init_cfg_revision;
	u32 geni_s_init_cfg_revision;
	u8  _reserved1[0x10 - 0x08];
	u32 geni_general_cfg;
	u32 geni_rx_fifo_ctrl;
	u8  _reserved2[0x20 - 0x18];
	u32 geni_force_default_reg;
	u32 geni_output_ctrl;
	u32 geni_cgc_ctrl;
	u32 geni_char_cfg;
	u32 geni_char_data_n;
	u8  _reserved3[0x40 - 0x34];
	u32 geni_status;
	u32 geni_test_bus_ctrl;
	u32 geni_ser_m_clk_cfg;
	u32 geni_ser_s_clk_cfg;
	u32 geni_prog_rom_ctrl_reg;
	u8  _reserved4[0x60 - 0x54];
	u32 geni_clk_ctrl_ro;
	u32 fifo_if_disable_ro;
	u32 geni_fw_revision_ro;
	u32 geni_s_fw_revision_ro;
	u32 geni_fw_multilock_protns_ro;
	u32 geni_fw_multilock_msa_ro;
	u32 geni_fw_multilock_sp_ro;
	u32 geni_clk_sel;
	u32 geni_dfs_if_cfg;
	u8 _reserved5[0x100 - 0x084];
	u32 geni_cfg_reg0;
	u32 geni_cfg_reg1;
	u32 geni_cfg_reg2;
	u32 geni_cfg_reg3;
	u32 geni_cfg_reg4;
	u32 geni_cfg_reg5;
	u32 geni_cfg_reg6;
	u32 geni_cfg_reg7;
	u32 geni_cfg_reg8;
	u32 geni_cfg_reg9;
	u32 geni_cfg_reg10;
	u32 geni_cfg_reg11;
	u32 geni_cfg_reg12;
	u32 geni_cfg_reg13;
	u32 geni_cfg_reg14;
	u32 geni_cfg_reg15;
	u32 geni_cfg_reg16;
	u32 geni_cfg_reg17;
	u32 geni_cfg_reg18;
	u8  _reserved6[0x200 - 0x14C];
	u32 geni_cfg_reg64;
	u32 geni_cfg_reg65;
	u32 geni_cfg_reg66;
	u32 geni_cfg_reg67;
	u32 geni_cfg_reg68;
	u32 geni_cfg_reg69;
	u32 geni_cfg_reg70;
	u32 geni_cfg_reg71;
	u32 geni_cfg_reg72;
	u32 spi_cpha;
	u32 geni_cfg_reg74;
	u32 proto_loopback_cfg;
	u32 spi_cpol;
	u32 i2c_noise_cancellation_ctl;
	u32 i2c_monitor_ctl;
	u32 geni_cfg_reg79;
	u32 geni_cfg_reg80;
	u32 geni_cfg_reg81;
	u32 geni_cfg_reg82;
	u32 spi_demux_output_inv;
	u32 spi_demux_sel;
	u32 geni_byte_granularity;
	u32 geni_dma_mode_en;
	u32 uart_tx_trans_cfg_reg;
	u32 geni_tx_packing_cfg0;
	u32 geni_tx_packing_cfg1;
	union {
		u32 uart_tx_word_len;
		u32 spi_word_len;
	};
	union {
		u32 uart_tx_stop_bit_len;
		u32 i2c_tx_trans_len;
		u32 spi_tx_trans_len;
	};
	union {
		u32 uart_tx_trans_len;
		u32 i2c_rx_trans_len;
		u32 spi_rx_trans_len;
	};
	u32 spi_pre_post_cmd_dly;
	u32 i2c_scl_counters;
	u32 geni_cfg_reg95;
	u32 uart_rx_trans_cfg;
	u32 geni_rx_packing_cfg0;
	u32 geni_rx_packing_cfg1;
	u32 uart_rx_word_len;
	u32 geni_cfg_reg100;
	u32 uart_rx_stale_cnt;
	u32 geni_cfg_reg102;
	u32 geni_cfg_reg103;
	u32 geni_cfg_reg104;
	u32 uart_tx_parity_cfg;
	u32 uart_rx_parity_cfg;
	u32 uart_manual_rfr;
	u32 geni_cfg_reg108;
	u32 geni_cfg_reg109;
	u32 geni_cfg_reg110;
	u8 _reserved7[0x600 - 0x2BC];
	u32 geni_m_cmd0;
	u32 geni_m_cmd_ctrl_reg;
	u8  _reserved8[0x10 - 0x08];
	u32 geni_m_irq_status;
	u32 geni_m_irq_enable;
	u32 geni_m_irq_clear;
	u32 geni_m_irq_en_set;
	u32 geni_m_irq_en_clear;
	u32 geni_m_cmd_err_status;
	u32 geni_m_fw_err_status;
	u8  _reserved9[0x30 - 0x2C];
	u32 geni_s_cmd0;
	u32 geni_s_cmd_ctrl_reg;
	u8  _reserved10[0x40 - 0x38];
	u32 geni_s_irq_status;
	u32 geni_s_irq_enable;
	u32 geni_s_irq_clear;
	u32 geni_s_irq_en_set;
	u32 geni_s_irq_en_clear;
	u8  _reserved11[0x700 - 0x654];
	u32 geni_tx_fifon;
	u8  _reserved12[0x780 - 0x704];
	u32 geni_rx_fifon;
	u8  _reserved13[0x800 - 0x784];
	u32 geni_tx_fifo_status;
	u32 geni_rx_fifo_status;
	u32 geni_tx_fifo_threshold;
	u32 geni_tx_watermark_reg;
	u32 geni_rx_watermark_reg;
	u32 geni_rx_rfr_watermark_reg;
	u8  _reserved14[0x900 - 0x818];
	u32 geni_gp_output_reg;
	u8  _reserved15[0x908 - 0x904];
	u32 geni_ios;
	u32 geni_timestamp;
	u32 geni_m_gp_length;
	u32 geni_s_gp_length;
	u8  _reserved16[0x920 - 0x918];
	u32 geni_hw_irq_en;
	u32 geni_hw_irq_ignore_on_active;
	u8  _reserved17[0x930 - 0x928];
	u32 geni_hw_irq_cmd_param_0;
	u8  _reserved18[0xA00 - 0x934];
	u32 geni_i3c_ibi_cfg_tablen;
	u8  _reserved19[0xA80 - 0xA04];
	u32 geni_i3c_ibi_status;
	u32 geni_i3c_ibi_rd_data;
	u32 geni_i3c_ibi_search_pattern;
	u32 geni_i3c_ibi_search_data;
	u32 geni_i3c_sw_ibi_en;
	u32 geni_i3c_sw_ibi_en_recover;
	u8 _reserved20[0xC30 - 0xA98];
	u32 dma_tx_ptr_l;
	u32 dma_tx_ptr_h;
	u32 dma_tx_attr;
	u32 dma_tx_length;
	u32 dma_tx_irq_stat;
	u32 dma_tx_irq_clr;
	u32 dma_tx_irq_en;
	u32 dma_tx_irq_en_set;
	u32 dma_tx_irq_en_clr;
	u32 dma_tx_length_in;
	u32 dma_tx_fsm_rst;
	u32 dma_tx_max_burst_size;
	u8  _reserved21[0xD30 - 0xC60];
	u32 dma_rx_ptr_l;
	u32 dma_rx_ptr_h;
	u32 dma_rx_attr;
	u32 dma_rx_length;
	u32 dma_rx_irq_stat;
	u32 dma_rx_irq_clr;
	u32 dma_rx_irq_en;
	u32 dma_rx_irq_en_set;
	u32 dma_rx_irq_en_clr;
	u32 dma_rx_length_in;
	u32 dma_rx_fsm_rst;
	u32 dma_rx_max_burst_size;
	u32 dma_rx_flush;
	u8  _reserved22[0xE14 - 0xD64];
	u32 se_irq_high_priority;
	u32 se_gsi_event_en;
	u32 se_irq_en;
	u32 dma_if_en_ro;
	u32 se_hw_param_0;
	u32 se_hw_param_1;
	u32 se_hw_param_2;
	u32 dma_general_cfg;
	u8  _reserved23[0x40 - 0x34];
	u32 dma_debug_reg0;
	u32 dma_test_bus_ctrl;
	u32 se_top_test_bus_ctrl;
	u8 _reserved24[0x1000 - 0x0E4C];
	u32 se_geni_fw_revision;
	u32 se_s_fw_revision;
	u8 _reserved25[0x10-0x08];
	u32 se_geni_cfg_ramn;
	u8 _reserved26[0x2000 - 0x1014];
	u32 se_geni_clk_ctrl;
	u32 se_dma_if_en;
	u32 se_fifo_if_disable;
	u32 se_geni_fw_multilock_protns;
	u32 se_geni_fw_multilock_msa;
	u32 se_geni_fw_multilock_sp;
};


check_member(qup_regs, geni_clk_sel, 0x7C);
check_member(qup_regs, geni_cfg_reg108, 0x2B0);
check_member(qup_regs, geni_dma_mode_en, 0x258);
check_member(qup_regs, geni_i3c_ibi_rd_data, 0xA84);
check_member(qup_regs, dma_test_bus_ctrl, 0xE44);
check_member(qup_regs, se_geni_cfg_ramn, 0x1010);
check_member(qup_regs, se_geni_fw_multilock_sp, 0x2014);


struct gsi_regs {
	u32 gsi_cfg;
	u8  _reserved1[0x8 - 0x4];
	u32 gsi_manager_mcs_code_ver;
	u8  _reserved2[0x14 - 0x10];
	u32 gsi_zeros;
	u8  _reserved3[0x18 - 0x14];
	u32 gsi_periph_base_lsb;
	u32 gsi_periph_base_msb;
	u8  _reserved4[0x60 - 0x20];
	u32 gsi_cgc_ctrl;
	u8  _reserved5[0x80 - 0x64];
	u32 gsi_msi_cacheattr;
	u8  _reserved6[0xB000 - 0x84];
	u32 gsi_mcs_cfg;
	u8  _reserved7[0xF018 - 0xB004];
	u32 ee_n_gsi_ee_generic_cmd;
	u8  _reserved8[0xF400 - 0xF01C];
	u32 gsi_ee_n_scratch_0_addr;
	u32 gsi_ee_n_scratch_1_addr;
	u8  _reserved9[0x4C000 - 0xF408];
	u32 gsi_inst_ramn;
	u8  _reserved10[0x4000];
};

check_member(gsi_regs, gsi_manager_mcs_code_ver, 0x8);
check_member(gsi_regs, gsi_zeros, 0x10);
check_member(gsi_regs, gsi_periph_base_lsb, 0x18);
check_member(gsi_regs, gsi_cgc_ctrl, 0x60);
check_member(gsi_regs, ee_n_gsi_ee_generic_cmd, 0xF018);
check_member(gsi_regs, gsi_ee_n_scratch_0_addr, 0xF400);
check_member(gsi_regs, gsi_ee_n_scratch_1_addr, 0xF404);
check_member(gsi_regs, gsi_inst_ramn, 0x4C000);


u32 qup_wait_for_m_irq(unsigned int bus);
u32 qup_wait_for_s_irq(unsigned int bus);
void qup_m_cancel_and_abort(unsigned int bus);
void qup_s_cancel_and_abort(unsigned int bus);
int qup_handle_transfer(unsigned int bus, const void *dout, void *din,
	int size, struct stopwatch *timeout);

#endif /* __SOC_COMMON_QCOM_QUP_SE_H__ */
