/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SOC_NVIDIA_TEGRA_I2C_H__
#define __SOC_NVIDIA_TEGRA_I2C_H__

#include <stdint.h>

void i2c_init(unsigned int bus);
void tegra_software_i2c_init(unsigned int bus);
void tegra_software_i2c_disable(unsigned int bus);

enum {
	/* Word 0 */
	IOHEADER_PROTHDRSZ_SHIFT = 28,
	IOHEADER_PROTHDRSZ_MASK = 0x3 << IOHEADER_PROTHDRSZ_SHIFT,
	IOHEADER_PKTID_SHIFT = 16,
	IOHEADER_PKTID_MASK = 0xff << IOHEADER_PKTID_SHIFT,
	IOHEADER_CONTROLLER_ID_SHIFT = 12,
	IOHEADER_CONTROLLER_ID_MASK = 0xf << IOHEADER_CONTROLLER_ID_SHIFT,
	IOHEADER_PROTOCOL_SHIFT = 4,
	IOHEADER_PROTOCOL_MASK = 0xf << IOHEADER_PROTOCOL_SHIFT,
	IOHEADER_PROTOCOL_I2C = 1 << IOHEADER_PROTOCOL_SHIFT,
	IOHEADER_PKTTYPE_SHIFT = 0,
	IOHEADER_PKTTYPE_MASK = 0x7 << IOHEADER_PKTTYPE_SHIFT,
	IOHEADER_PKTTYPE_REQUEST = 0 << IOHEADER_PKTTYPE_SHIFT,
	IOHEADER_PKTTYPE_RESPONSE = 1 << IOHEADER_PKTTYPE_SHIFT,
	IOHEADER_PKTTYPE_INTERRUPT = 2 << IOHEADER_PKTTYPE_SHIFT,
	IOHEADER_PKTTYPE_STOP = 3 << IOHEADER_PKTTYPE_SHIFT,

	/* Word 1 */
	IOHEADER_PAYLOADSIZE_SHIFT = 0,
	IOHEADER_PAYLOADSIZE_MASK = 0xfff << IOHEADER_PAYLOADSIZE_SHIFT
};

enum {
	IOHEADER_I2C_REQ_RESP_FREQ_MASK = 0x1 << 25,
	IOHEADER_I2C_REQ_RESP_FREQ_END = 0 << 25,
	IOHEADER_I2C_REQ_RESP_FREQ_EACH = 1 << 25,
	IOHEADER_I2C_REQ_RESP_ENABLE = 0x1 << 24,
	IOHEADER_I2C_REQ_HS_MODE = 0x1 << 22,
	IOHEADER_I2C_REQ_CONTINUE_ON_NACK = 0x1 << 21,
	IOHEADER_I2C_REQ_SEND_START_BYTE = 0x1 << 20,
	IOHEADER_I2C_REQ_READ = 0x1 << 19,
	IOHEADER_I2C_REQ_ADDR_MODE_MASK = 0x1 << 18,
	IOHEADER_I2C_REQ_ADDR_MODE_7BIT = 0 << 18,
	IOHEADER_I2C_REQ_ADDR_MODE_10BIT = 1 << 18,
	IOHEADER_I2C_REQ_IE = 0x1 << 17,
	IOHEADER_I2C_REQ_REPEAT_START = 0x1 << 16,
	IOHEADER_I2C_REQ_STOP = 0x0 << 16,
	IOHEADER_I2C_REQ_CONTINUE_XFER = 0x1 << 15,
	IOHEADER_I2C_REQ_HS_MASTER_ADDR_SHIFT = 12,
	IOHEADER_I2C_REQ_HS_MASTER_ADDR_MASK =
		0x7 << IOHEADER_I2C_REQ_HS_MASTER_ADDR_SHIFT,
	IOHEADER_I2C_REQ_SLAVE_ADDR_SHIFT = 0,
	IOHEADER_I2C_REQ_SLAVE_ADDR_MASK =
		0x3ff << IOHEADER_I2C_REQ_SLAVE_ADDR_SHIFT
};

enum {
	I2C_CNFG_MSTR_CLR_BUS_ON_TIMEOUT = 0x1 << 15,
	I2C_CNFG_DEBOUNCE_CNT_SHIFT = 12,
	I2C_CNFG_DEBOUNCE_CNT_MASK = 0x7 << I2C_CNFG_DEBOUNCE_CNT_SHIFT,
	I2C_CNFG_NEW_MASTER_FSM = 0x1 << 11,
	I2C_CNFG_PACKET_MODE_EN = 0x1 << 10,
	I2C_CNFG_SEND = 0x1 << 9,
	I2C_CNFG_NOACK = 0x1 << 8,
	I2C_CNFG_CMD2 = 0x1 << 7,
	I2C_CNFG_CMD1 = 0x1 << 6,
	I2C_CNFG_START = 0x1 << 5,
	I2C_CNFG_SLV2_SHIFT = 4,
	I2C_CNFG_SLV2_MASK = 0x1 << I2C_CNFG_SLV2_SHIFT,
	I2C_CNFG_LENGTH_SHIFT = 1,
	I2C_CNFG_LENGTH_MASK = 0x7 << I2C_CNFG_LENGTH_SHIFT,
	I2C_CNFG_A_MOD = 0x1 << 0,
};

enum {
	I2C_PKT_STATUS_COMPLETE = 0x1 << 24,
	I2C_PKT_STATUS_PKT_ID_SHIFT = 16,
	I2C_PKT_STATUS_PKT_ID_MASK = 0xff << I2C_PKT_STATUS_PKT_ID_SHIFT,
	I2C_PKT_STATUS_BYTENUM_SHIFT = 4,
	I2C_PKT_STATUS_BYTENUM_MASK = 0xfff << I2C_PKT_STATUS_BYTENUM_SHIFT,
	I2C_PKT_STATUS_NOACK_ADDR = 0x1 << 3,
	I2C_PKT_STATUS_NOACK_DATA = 0x1 << 2,
	I2C_PKT_STATUS_ARB_LOST = 0x1 << 1,
	I2C_PKT_STATUS_BUSY = 0x1 << 0
};

enum {
	I2C_FIFO_STATUS_TX_FIFO_EMPTY_CNT_SHIFT = 4,
	I2C_FIFO_STATUS_TX_FIFO_EMPTY_CNT_MASK =
		0xf << I2C_FIFO_STATUS_TX_FIFO_EMPTY_CNT_SHIFT,
	I2C_FIFO_STATUS_RX_FIFO_FULL_CNT_SHIFT = 0,
	I2C_FIFO_STATUS_RX_FIFO_FULL_CNT_MASK =
		0xf << I2C_FIFO_STATUS_RX_FIFO_FULL_CNT_SHIFT
};

enum {
	I2C_BUS_CLEAR_CONFIG_BC_SCLK_THRESHOLD_SHIFT = 16,
	I2C_BUS_CLEAR_CONFIG_BC_SCLK_THRESHOLD_MASK =
		0xff << I2C_BUS_CLEAR_CONFIG_BC_SCLK_THRESHOLD_SHIFT,
	I2C_BUS_CLEAR_CONFIG_BC_STOP_COND_STOP = 0x1 << 2,
	I2C_BUS_CLEAR_CONFIG_BC_TERMINATE_IMMEDIATE = 0x1 << 1,
	I2C_BUS_CLEAR_CONFIG_BC_ENABLE = 0x1 << 0,

	I2C_BUS_CLEAR_STATUS_CLEARED = 0x1 << 0,

	I2C_CONFIG_LOAD_MSTR_CONFIG_LOAD_ENABLE = 0x1 << 0
};

struct tegra_i2c_bus_info {
	void *base;
	uint32_t reset_bit;
	void (*reset_func)(u32 bit);
};

extern struct tegra_i2c_bus_info tegra_i2c_info[];

struct tegra_i2c_regs {
	uint32_t cnfg;
	uint32_t cmd_addr0;
	uint32_t cmd_addr1;
	uint32_t cmd_data1;
	uint32_t cmd_data2;
	uint8_t _rsv0[8];
	uint32_t status;
	uint32_t sl_cnfg;
	uint32_t sl_rcvd;
	uint32_t sl_status;
	uint32_t sl_addr1;
	uint32_t sl_addr2;
	uint32_t tlow_sext;
	uint8_t _rsv1[4];
	uint32_t sl_delay_count;
	uint32_t sl_int_mask;
	uint32_t sl_int_source;
	uint32_t sl_int_set;
	uint8_t _rsv2[4];
	uint32_t tx_packet_fifo;
	uint32_t rx_fifo;
	uint32_t packet_transfer_status;
	uint32_t fifo_control;
	uint32_t fifo_status;
	uint32_t interrupt_mask;
	uint32_t interrupt_status;
	uint32_t clk_divisor;
	uint32_t interrupt_source;
	uint32_t interrupt_set;
	uint32_t slv_tx_packet_fifo;
	uint32_t slv_rx_fifo;
	uint32_t slv_packet_status;
	uint32_t bus_clear_config;
	uint32_t bus_clear_status;
	uint32_t config_load;
};
check_member(tegra_i2c_regs, config_load, 0x8C);

extern const unsigned int num_i2c_buses;

#endif	/* __SOC_NVIDIA_TEGRA_I2C_H__ */
