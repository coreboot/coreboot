/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/i2c.h>
#include <soc/ramstage.h>
#include <soc/reg_access.h>

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
			die("ERROR - I2C failed to disable!\n");
		status = regs->ic_enable_status;
	}

	/* Clear any pending interrupts */
	status = regs->ic_clr_intr;
}

int platform_i2c_transfer(unsigned bus, struct i2c_seg *segments, int count)
{
	uint8_t *buffer;
	int bytes_transferred;
	uint8_t chip;
	uint32_t cmd;
	int length;
	int read_length;
	I2C_REGS *regs;
	uint32_t status;
	uint32_t timeout;

	regs = get_i2c_address();

	/* Disable the I2C controller to get access to the registers */
	i2c_disable(regs);

	/* Set the slave address */
	ASSERT (count > 0);
	ASSERT (segments != NULL);
	ASSERT (segments->read == 0);

	/* Clear the start and stop detection */
	status = regs->ic_clr_start_det;
	status = regs->ic_clr_stop_det;

	/* Set addressing mode to 7-bit and fast mode */
	cmd = regs->ic_con;
	cmd &= ~(IC_CON_10B | IC_CON_SPEED);
	cmd |= IC_CON_RESTART_EN | IC_CON_7B | IC_CON_SPEED_100_KHz
		| IC_CON_MASTER_MODE;
	regs->ic_con = cmd;

	/* Set the target chip address */
	chip = segments->chip;
	regs->ic_tar = chip;

	/* Enable the I2C controller */
	regs->ic_enable = IC_ENABLE_CONTROLLER;

	/* Clear the interrupts */
	status = regs->ic_clr_rx_under;
	status = regs->ic_clr_rx_over;
	status = regs->ic_clr_tx_over;
	status = regs->ic_clr_tx_abrt;

	/* Process each of the segments */
	bytes_transferred = 0;
	read_length = 0;
	buffer = NULL;
	while (count-- > 0) {
		buffer = segments->buf;
		length = segments->len;
		ASSERT (buffer != NULL);
		ASSERT (length >= 1);
		ASSERT (segments->chip == chip);

		if (segments->read) {
			/* Place read commands into the FIFO */
			read_length = length;
			while (length > 0) {
				/* Send stop bit after last byte */
				cmd = IC_DATA_CMD_READ;
				if ((count == 0) && (length == 1))
					cmd |= IC_DATA_CMD_STOP;

				/* Place read command in transmit FIFO */
				regs->ic_data_cmd = cmd;
				length--;
			}
		} else {
			/* Write the data into the FIFO */
			while (length > 0) {
				/* End of the transaction? */
				cmd = IC_DATA_CMD_WRITE | *buffer++;
				if ((count == 0) && (length == 1))
					cmd |= IC_DATA_CMD_STOP;

				/* Place a data byte into the FIFO */
				regs->ic_data_cmd = cmd;
				length--;
				bytes_transferred++;
			}
		}
		segments++;
	}

	/* Wait for the end of the transaction */
	timeout = 1 * 1000 * 1000;
	do {
		status = regs->ic_raw_intr_stat;
		if (status & IC_INTR_STOP_DET)
			break;
		if ((status & (IC_INTR_RX_OVER | IC_INTR_RX_UNDER
				| IC_INTR_TX_ABRT | IC_INTR_TX_OVER))
			|| (timeout == 0)) {
			if (timeout == 0)
				printk (BIOS_ERR,
					"ERROR - I2C stop bit not received!\n");
			if (status & IC_INTR_RX_OVER)
				printk (BIOS_ERR,
					"ERROR - I2C receive overrun!\n");
			if (status & IC_INTR_RX_UNDER)
				printk (BIOS_ERR,
					"ERROR - I2C receive underrun!\n");
			if (status & IC_INTR_TX_ABRT)
				printk (BIOS_ERR,
					"ERROR - I2C transmit abort!\n");
			if (status & IC_INTR_TX_OVER)
				printk (BIOS_ERR,
					"ERROR - I2C transmit overrun!\n");
			i2c_disable(regs);
			return -1;
		}
		timeout--;
		udelay(1);
	} while (1);

	/* Finish reading the data bytes */
	while (read_length > 0) {
		status = regs->ic_status;
		*buffer++ = (uint8_t)regs->ic_data_cmd;
		read_length--;
		bytes_transferred++;
		status = regs->ic_status;
	}

	return bytes_transferred;
}
