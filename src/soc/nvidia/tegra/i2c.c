/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <string.h>
#include <soc/addressmap.h>
#include <stdint.h>

#include "i2c.h"

static void do_bus_clear(int bus)
{
	struct tegra_i2c_bus_info *info = &tegra_i2c_info[bus];
	struct tegra_i2c_regs * const regs = info->base;
	uint32_t bc;
	int i, timeout_ms = 10;

	// BUS CLEAR regs (from TRM):
	// 1. Reset the I2C controller (already done)
	// 2. Set the # of clock pulses required (using default of 9)
	// 3. Select STOP condition (using default of 1 = STOP)
	// 4. Set TERMINATE condition (1 = IMMEDIATE)
	bc = read32(&regs->bus_clear_config);
	bc |= I2C_BUS_CLEAR_CONFIG_BC_TERMINATE_IMMEDIATE;
	write32(&regs->bus_clear_config, bc);
	// 4.1 Set MSTR_CONFIG_LOAD and wait for clear
	write32(&regs->config_load, I2C_CONFIG_LOAD_MSTR_CONFIG_LOAD_ENABLE);
	for (i = 0; i < timeout_ms * 10 && (read32(&regs->config_load) &
			I2C_CONFIG_LOAD_MSTR_CONFIG_LOAD_ENABLE); i++) {
		printk(BIOS_DEBUG, "%s: wait for MSTR_CONFIG_LOAD to clear\n",
			__func__);
		udelay(100);
	}
	// 5. Set ENABLE to start the bus clear op
	write32(&regs->bus_clear_config, bc | I2C_BUS_CLEAR_CONFIG_BC_ENABLE);
	for (i = 0; i < timeout_ms * 10 && (read32(&regs->bus_clear_config) &
			I2C_BUS_CLEAR_CONFIG_BC_ENABLE); i++) {
		printk(BIOS_DEBUG, "%s: wait for bus clear completion\n",
			__func__);
		udelay(100);
	}
}

static int tegra_i2c_send_recv(int bus, int read,
			       uint32_t *headers, int header_words,
			       uint8_t *data, int data_len)
{
	struct tegra_i2c_bus_info *info = &tegra_i2c_info[bus];
	struct tegra_i2c_regs * const regs = info->base;

	while (data_len) {
		uint32_t status = read32(&regs->fifo_status);
		int tx_empty = status & I2C_FIFO_STATUS_TX_FIFO_EMPTY_CNT_MASK;
		tx_empty >>= I2C_FIFO_STATUS_TX_FIFO_EMPTY_CNT_SHIFT;
		int rx_full = status & I2C_FIFO_STATUS_RX_FIFO_FULL_CNT_MASK;
		rx_full >>= I2C_FIFO_STATUS_RX_FIFO_FULL_CNT_SHIFT;

		while (header_words && tx_empty) {
			write32(&regs->tx_packet_fifo, *headers++);
			header_words--;
			tx_empty--;
		}

		if (!header_words) {
			if (read) {
				while (data_len && rx_full) {
					uint32_t word = read32(&regs->rx_fifo);
					int todo = MIN(data_len, sizeof(word));

					memcpy(data, &word, todo);
					data_len -= todo;
					data += sizeof(word);
					rx_full--;
				}
			} else {
				while (data_len && tx_empty) {
					uint32_t word;
					int todo = MIN(data_len, sizeof(word));

					memcpy(&word, data, todo);
					write32(&regs->tx_packet_fifo, word);
					data_len -= todo;
					data += sizeof(word);
					tx_empty--;
				}
			}
		}

		uint32_t transfer_status =
			read32(&regs->packet_transfer_status);

		if (transfer_status & I2C_PKT_STATUS_NOACK_ADDR) {
			printk(BIOS_ERR,
			       "%s: The address was not acknowledged.\n",
			       __func__);
			info->reset_func(info->reset_bit);
			i2c_init(bus);
			return -1;
		} else if (transfer_status & I2C_PKT_STATUS_NOACK_DATA) {
			printk(BIOS_ERR,
			       "%s: The data was not acknowledged.\n",
			       __func__);
			info->reset_func(info->reset_bit);
			i2c_init(bus);
			return -1;
		} else if (transfer_status & I2C_PKT_STATUS_ARB_LOST) {
			printk(BIOS_ERR,
			       "%s: Lost arbitration.\n",
			       __func__);
			info->reset_func(info->reset_bit);

			/* Use Tegra bus clear registers to unlock SDA */
			do_bus_clear(bus);

			/* re-init i2c controller */
			i2c_init(bus);

			/* Return w/error, let caller decide what to do */
			return -1;
		}
	}

	return 0;
}

static int tegra_i2c_request(int bus, unsigned int chip, int cont, int restart,
			     int read, void *data, int data_len)
{
	uint32_t headers[3];

	if (restart && cont) {
		printk(BIOS_ERR, "%s: Repeat start and continue xfer are "
			"mutually exclusive.\n", __func__);
		return -1;
	}

	headers[0] = (0 << IOHEADER_PROTHDRSZ_SHIFT) |
		     (1 << IOHEADER_PKTID_SHIFT) |
		     (bus << IOHEADER_CONTROLLER_ID_SHIFT) |
		     IOHEADER_PROTOCOL_I2C | IOHEADER_PKTTYPE_REQUEST;

	headers[1] = (data_len - 1) << IOHEADER_PAYLOADSIZE_SHIFT;

	uint32_t slave_addr = (chip << 1) | (read ? 1 : 0);
	headers[2] = IOHEADER_I2C_REQ_ADDR_MODE_7BIT |
		     (slave_addr << IOHEADER_I2C_REQ_SLAVE_ADDR_SHIFT);
	if (read)
		headers[2] |= IOHEADER_I2C_REQ_READ;
	if (restart)
		headers[2] |= IOHEADER_I2C_REQ_REPEAT_START;
	if (cont)
		headers[2] |= IOHEADER_I2C_REQ_CONTINUE_XFER;

	return tegra_i2c_send_recv(bus, read, headers, ARRAY_SIZE(headers),
				   data, data_len);
}

static int i2c_transfer_segment(unsigned int bus, unsigned int chip, int restart,
				int read, void *buf, int len)
{
	const uint32_t max_payload =
		(IOHEADER_PAYLOADSIZE_MASK + 1) >> IOHEADER_PAYLOADSIZE_SHIFT;

	while (len) {
		int todo = MIN(len, max_payload);
		int cont = (todo < len);
		if (tegra_i2c_request(bus, chip, cont, restart,
				      read, buf, todo))
			return -1;
		len -= todo;
		buf += todo;
	}
	return 0;
}

int platform_i2c_transfer(unsigned int bus, struct i2c_msg *segments, int count)
{
	struct i2c_msg *seg = segments;
	int i;

	if (bus >= num_i2c_buses) {
		printk(BIOS_ERR, "%s: ERROR: invalid I2C bus (%u)\n", __func__,
		       bus);
		return -1;
	}

	for (i = 0; i < count; seg++, i++) {
		if (i2c_transfer_segment(bus, seg->slave, i < count - 1,
					 seg->flags & I2C_M_RD,
					 seg->buf, seg->len))
			return -1;
	}
	return 0;
}

void i2c_init(unsigned int bus)
{
	struct tegra_i2c_regs *regs;

	if (bus >= num_i2c_buses) {
		printk(BIOS_ERR, "%s: ERROR: invalid I2C bus (%u)\n", __func__,
		       bus);
		return;
	}

	regs = tegra_i2c_info[bus].base;

	write32(&regs->cnfg, I2C_CNFG_PACKET_MODE_EN);
}
