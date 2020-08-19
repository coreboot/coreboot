/*
 * Copyright (c) 2020 Qualcomm Technologies.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* For simplicity sake let's rely on coreboot initializing the UART. */
#include <config.h>
#include <libpayload.h>
#include <sys/types.h>

#define GENI_STATUS_M_GENI_CMD_ACTIVE_MASK	0x1
#define RX_FIFO_WC_MSK	0x1FFFFFF
#define START_UART_TX	0x8000000

union proto_word_len {
	u32 uart_tx_word_len;
	u32 spi_word_len;
};

union proto_tx_trans_len {
	u32 uart_tx_stop_bit_len;
	u32 i2c_tx_trans_len;
	u32 spi_tx_trans_len;
};

union proto_rx_trans_len {
	u32 uart_tx_trans_len;
	u32 i2c_rx_trans_len;
	u32 spi_rx_trans_len;
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
	union proto_word_len word_len;
	union proto_tx_trans_len tx_trans_len;
	union proto_rx_trans_len rx_trans_len;
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

static struct console_input_driver consin = {
	.havekey = serial_havechar,
	.getchar = serial_getchar,
	.input_type = CONSOLE_INPUT_TYPE_UART,
};

static struct console_output_driver consout = {
	.putchar = serial_putchar,
};

static struct qup_regs *uart_base_address(void)
{
	const struct cb_serial *const serial = phys_to_virt(lib_sysinfo.cb_serial);
	return phys_to_virt(serial->baseaddr);
}

static void uart_qupv3_tx_flush(void)
{
	struct qup_regs *regs = uart_base_address();

	while (read32(&regs->geni_status) & GENI_STATUS_M_GENI_CMD_ACTIVE_MASK)
		;
}

static unsigned char uart_qupv3_rx_byte(void)
{
	struct qup_regs *regs = uart_base_address();

	if (read32(&regs->geni_rx_fifo_status) & RX_FIFO_WC_MSK)
		return read32(&regs->geni_rx_fifon) & 0xFF;

	return 0;
}

static void uart_qupv3_tx_byte(unsigned char data)
{
	struct qup_regs *regs = uart_base_address();

	uart_qupv3_tx_flush();

	write32(&regs->rx_trans_len.uart_tx_trans_len, 1);
	/* Start TX */
	write32(&regs->geni_m_cmd0, START_UART_TX);
	write32(&regs->geni_tx_fifon, data);
}

void serial_putchar(unsigned int data)
{
	if (data == 0xa)
		uart_qupv3_tx_byte(0xd);
	uart_qupv3_tx_byte(data);
}

int serial_havechar(void)
{
	struct qup_regs *regs = uart_base_address();

	if (read32(&regs->geni_rx_fifo_status) & RX_FIFO_WC_MSK)
		return 1;

	return 0;
}

int serial_getchar(void)
{
	return uart_qupv3_rx_byte();
}

void serial_console_init(void)
{
	if (!lib_sysinfo.cb_serial)
		return;

	console_add_output_driver(&consout);
	console_add_input_driver(&consin);
}
