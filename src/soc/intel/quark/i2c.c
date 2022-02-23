/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c_simple.h>
#include <device/pci.h>
#include <soc/i2c.h>
#include <soc/ramstage.h>
#include <soc/reg_access.h>
#include <timer.h>

static void i2c_disable(I2C_REGS *regs)
{
	uint32_t status;
	uint32_t timeout;

	/* Disable I2C controller */
	regs->ic_enable = 0;

	/* Wait for the enable bit to clear */
	timeout = 1 * 1000 * 1000;
	status = regs->ic_enable_status;
	while (status & IC_ENABLE_CONTROLLER) {
		udelay(1);
		if (--timeout == 0)
			die_with_post_code(POST_HW_INIT_FAILURE,
					   "ERROR - I2C failed to disable!\n");
		status = regs->ic_enable_status;
	}

	/* Clear any pending interrupts */
	status = regs->ic_clr_intr;
}

static int platform_i2c_write(uint32_t restart, uint8_t *tx_buffer, int length,
	uint32_t stop, uint8_t *rx_buffer, struct stopwatch *timeout)
{
	int bytes_transferred;
	uint32_t cmd;
	I2C_REGS *regs;
	uint32_t status;

	ASSERT(tx_buffer != NULL);
	ASSERT(timeout != NULL);
	regs = get_i2c_address();

	/* Fill the FIFO with the write operation */
	bytes_transferred = 0;
	do {
		status = regs->ic_raw_intr_stat;

		/* Check for errors */
		if (status & (IC_INTR_RX_OVER | IC_INTR_RX_UNDER
				| IC_INTR_TX_ABRT | IC_INTR_TX_OVER)) {
			i2c_disable(regs);
			if (CONFIG(I2C_DEBUG))
				printk(BIOS_ERR,
					"0x%08x: ic_raw_intr_stat, I2C write error!\n",
					status);
			return -1;
		}

		/* Check for timeout */
		if (stopwatch_expired(timeout)) {
			if (CONFIG(I2C_DEBUG))
				printk(BIOS_ERR,
					"0x%08x: ic_raw_intr_stat, I2C write timeout!\n",
					status);
			return -1;
		}

		/* Receive any available data */
		status = regs->ic_status;
		if (rx_buffer != NULL) {
			while (status & IC_STATUS_RFNE) {
				*rx_buffer++ = (uint8_t)regs->ic_data_cmd;
				bytes_transferred++;
				status = regs->ic_status;
			}
		}

		/* Determine if space is available in the FIFO */
		if (status & IC_STATUS_TFNF) {
			/* End of the transaction? */
			cmd = IC_DATA_CMD_WRITE | *tx_buffer++ | restart;
			if (length == 1)
				cmd |= stop;
			restart = 0;

			/* Place a data byte into the FIFO */
			regs->ic_data_cmd = cmd;
			length--;
			bytes_transferred++;
		} else
			udelay(1);
	} while (length > 0);
	return bytes_transferred;
}

static int platform_i2c_read(uint32_t restart, uint8_t *rx_buffer, int length,
	uint32_t stop, struct stopwatch *timeout)
{
	int bytes_transferred;
	uint32_t cmd;
	int fifo_bytes;
	I2C_REGS *regs;
	uint32_t status;

	ASSERT(rx_buffer != NULL);
	ASSERT(timeout != NULL);
	regs = get_i2c_address();

	/* Empty the FIFO */
	status = regs->ic_status;
	while (status & IC_STATUS_RFNE) {
		(void)regs->ic_data_cmd;
		status = regs->ic_status;
	}

	/* Fill the FIFO with read commands */
	fifo_bytes = MIN(length, 16);
	bytes_transferred = 0;
	while (length > 0) {
		status = regs->ic_raw_intr_stat;

		/* Check for errors */
		if (status & (IC_INTR_RX_OVER | IC_INTR_RX_UNDER
				| IC_INTR_TX_ABRT | IC_INTR_TX_OVER)) {
			i2c_disable(regs);
			if (CONFIG(I2C_DEBUG))
				printk(BIOS_ERR,
					"0x%08x: ic_raw_intr_stat, I2C read error!\n",
					status);
			return -1;
		}

		/* Check for timeout */
		if (stopwatch_expired(timeout)) {
			if (CONFIG(I2C_DEBUG))
				printk(BIOS_ERR,
					"0x%08x: ic_raw_intr_stat, I2C read timeout!\n",
					status);
			return -1;
		}

		/* Receive any available data */
		status = regs->ic_status;
		if (status & IC_STATUS_RFNE) {
			/* Save the next data byte, removed from the RX FIFO */
			*rx_buffer++ = (uint8_t)regs->ic_data_cmd;
			bytes_transferred++;
		}

		if ((status & IC_STATUS_TFNF)
			|| ((status & IC_STATUS_RFNE) && (fifo_bytes > 0))) {
			/* End of the transaction? */
			cmd = IC_DATA_CMD_READ | restart;
			if (length == 1)
				cmd |= stop;
			restart = 0;

			/* Place a read command into the TX FIFO */
			regs->ic_data_cmd = cmd;
			if (fifo_bytes > 0)
				fifo_bytes--;
			length--;
		} else
			udelay(1);
	}
	return bytes_transferred;
}

int platform_i2c_transfer(unsigned int bus, struct i2c_msg *segment,
			  int seg_count)
{
	int bytes_transferred;
	uint8_t chip;
	uint32_t cmd;
	int data_bytes;
	int index;
	int length;
	I2C_REGS *regs;
	uint32_t restart;
	uint8_t *rx_buffer;
	uint32_t status;
	uint32_t stop;
	struct stopwatch timeout;
	int total_bytes;
	uint8_t *tx_buffer;
	int tx_bytes;

	if (CONFIG(I2C_DEBUG)) {
		for (index = 0; index < seg_count;) {
			if (index == 0)
				printk(BIOS_ERR, "I2C Start\n");
			printk(BIOS_ERR,
				"I2C segment[%d]: %s 0x%02x %s %p, 0x%08x bytes\n",
				index,
				(segment[index].flags & I2C_M_RD) ? "Read from" : "Write to",
				segment[index].slave,
				(segment[index].flags & I2C_M_RD) ? "to " : "from",
				segment[index].buf,
				segment[index].len);
			printk(BIOS_ERR, "I2C %s\n",
				(++index >= seg_count) ? "Stop" : "Restart");
		}
	}

	regs = get_i2c_address();

	/* Disable the I2C controller to get access to the registers */
	i2c_disable(regs);

	/* Set the slave address */
	ASSERT(seg_count > 0);
	ASSERT(segment != NULL);

	/* Clear the start and stop detection */
	status = regs->ic_clr_start_det;
	status = regs->ic_clr_stop_det;

	/* Set addressing mode to 7-bit and fast mode */
	cmd = regs->ic_con;
	cmd &= ~(IC_CON_10B | IC_CON_SPEED);
	cmd |= IC_CON_RESTART_EN | IC_CON_7B | IC_CON_SPEED_400_KHz
		| IC_CON_MASTER_MODE;
	regs->ic_con = cmd;

	/* Set the target chip address */
	chip = segment->slave;
	regs->ic_tar = chip;

	/* Enable the I2C controller */
	regs->ic_enable = IC_ENABLE_CONTROLLER;

	/* Clear the interrupts */
	status = regs->ic_clr_rx_under;
	status = regs->ic_clr_rx_over;
	status = regs->ic_clr_tx_over;
	status = regs->ic_clr_tx_abrt;

	/* Start the timeout */
	stopwatch_init_usecs_expire(&timeout, CONFIG_I2C_TRANSFER_TIMEOUT_US);

	/* Process each of the segments */
	total_bytes = 0;
	tx_bytes = 0;
	bytes_transferred = 0;
	rx_buffer = NULL;
	restart = 0;
	index = 0;
	while (index++ < seg_count) {
		length = segment->len;
		total_bytes += length;
		ASSERT(segment->buf != NULL);
		ASSERT(length >= 1);
		ASSERT(segment->slave == chip);

		/* Determine if this is the last segment of the transaction */
		stop = (index == seg_count) ? IC_DATA_CMD_STOP : 0;

		/* Fill the FIFO with the necessary command bytes */
		if (segment->flags & I2C_M_RD) {
			/* Place read commands into the FIFO */
			rx_buffer = segment->buf;
			data_bytes = platform_i2c_read(restart, rx_buffer,
				length, stop, &timeout);

			/* Return any detected error */
			if (data_bytes < 0) {
				if (CONFIG(I2C_DEBUG))
					printk(BIOS_ERR,
						"I2C segment[%d] failed\n",
						index);
				return data_bytes;
			}
			bytes_transferred += data_bytes;
		} else {
			/* Write the data into the FIFO */
			tx_buffer = segment->buf;
			tx_bytes += length;
			data_bytes = platform_i2c_write(restart, tx_buffer,
				length, stop, rx_buffer, &timeout);

			/* Return any detected error */
			if (data_bytes < 0) {
				if (CONFIG(I2C_DEBUG))
					printk(BIOS_ERR,
						"I2C segment[%d] failed\n",
						index);
				return data_bytes;
			}
			bytes_transferred += data_bytes;
		}
		segment++;
		restart = IC_DATA_CMD_RESTART;
	}

	/* Wait for the end of the transaction */
	if (rx_buffer != NULL)
		rx_buffer += bytes_transferred - tx_bytes;
	do {
		/* Receive any available data */
		status = regs->ic_status;
		if ((rx_buffer != NULL) && (status & IC_STATUS_RFNE)) {
			*rx_buffer++ = (uint8_t)regs->ic_data_cmd;
			bytes_transferred++;
		} else {
			status = regs->ic_raw_intr_stat;
			if ((total_bytes == bytes_transferred)
				&& (status & IC_INTR_STOP_DET))
				break;

			/* Check for errors */
			if (status & (IC_INTR_RX_OVER | IC_INTR_RX_UNDER
					| IC_INTR_TX_ABRT | IC_INTR_TX_OVER)) {
				i2c_disable(regs);
				if (CONFIG(I2C_DEBUG)) {
					printk(BIOS_ERR,
						"0x%08x: ic_raw_intr_stat, I2C read error!\n",
						status);
					printk(BIOS_ERR,
						"I2C segment[%d] failed\n",
						seg_count - 1);
				}
				return -1;
			}

			/* Check for timeout */
			if (stopwatch_expired(&timeout)) {
				if (CONFIG(I2C_DEBUG)) {
					printk(BIOS_ERR,
						"0x%08x: ic_raw_intr_stat, I2C read timeout!\n",
						status);
					printk(BIOS_ERR,
						"I2C segment[%d] failed\n",
						seg_count - 1);
				}
				return -1;
			}

			/* Delay for a while */
			udelay(1);
		}
	} while (1);
	i2c_disable(regs);
	regs->ic_tar = 0;

	/* Return the number of bytes transferred */
	if (CONFIG(I2C_DEBUG))
		printk(BIOS_ERR, "0x%08x: bytes transferred\n",
			bytes_transferred);
	return bytes_transferred;
}
