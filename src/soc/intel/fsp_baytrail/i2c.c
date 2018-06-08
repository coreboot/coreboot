/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Siemens AG
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

#include <device/pci.h>
#include <soc/baytrail.h>
#include <soc/pci_devs.h>
#include <soc/iosf.h>
#include <delay.h>
#include <soc/i2c.h>

/* Wait for the transmit FIFO till there is at least one slot empty.
 * FIFO stall due to transmit abort will be checked and resolved
 */
static int wait_tx_fifo(char *base_adr)
{
	int i;
	u32 as;

	as = read32(base_adr + I2C_ABORT_SOURCE) & 0x1ffff;
	if (as) {
		/* Reading back I2C_CLR_TX_ABRT resets abort lock on TX FIFO */
		i = read32(base_adr + I2C_CLR_TX_ABRT);
		return I2C_ERR_ABORT | as;
	}

	/* Wait here for a free slot in TX-FIFO */
	i = I2C_TIMEOUT_US;
	while (!(read32(base_adr + I2C_STATUS) & I2C_TFNF)) {
		udelay(1);
		if (!--i)
			return I2C_ERR_TIMEOUT;
	}

	return I2C_SUCCESS;
}

/* Wait for the receive FIFO till there is at least one valid entry to read.
 * FIFO stall due to transmit abort will be checked and resolved
 */
static int wait_rx_fifo(char *base_adr)
{
	int i;
	u32 as;

	as = read32(base_adr + I2C_ABORT_SOURCE) & 0x1ffff;
	if (as) {
		/* Reading back I2C_CLR_TX_ABRT resets abort lock on TX FIFO */
		i = read32(base_adr + I2C_CLR_TX_ABRT);
		return I2C_ERR_ABORT | as;
	}

	/* Wait here for a received entry in RX-FIFO */
	i = I2C_TIMEOUT_US;
	while (!(read32(base_adr + I2C_STATUS) & I2C_RFNE)) {
		udelay(1);
		if (!--i)
			return I2C_ERR_TIMEOUT;
	}

	return I2C_SUCCESS;
}

/* When there will be a fast switch between send and receive, one have
 * to wait until the first operation is completely finished
 * before starting the second operation
 */
static int wait_for_idle(char *base_adr)
{
	int i;
	int status;

	/* For IDLE, increase timeout by ten times */
	i = I2C_TIMEOUT_US * 10;
	status = read32(base_adr + I2C_STATUS);
	while (((status & I2C_MST_ACTIVITY) || (!(status & I2C_TFE)))) {
		status = read32(base_adr + I2C_STATUS);
		udelay(1);
		if (!--i)
			return I2C_ERR_TIMEOUT;
	}

	return I2C_SUCCESS;
}

/** \brief Enables I2C-controller, sets up BAR and timing parameters
 * @param   bus Number of the I2C-controller to use (0...6)
 * @return  I2C_SUCCESS on success, otherwise error code
 */
int i2c_init(unsigned bus)
{
	struct device *dev;
	int base_adr[7] = {I2C0_MEM_BASE, I2C1_MEM_BASE, I2C2_MEM_BASE,
			   I2C3_MEM_BASE, I2C4_MEM_BASE, I2C5_MEM_BASE,
			   I2C6_MEM_BASE};
	char *base_ptr;

	/* Ensure the desired device is valid */
	if (bus >= ARRAY_SIZE(base_adr)) {
		printk(BIOS_ERR, "I2C: Only I2C controllers 0...6 are available.\n");
		return I2C_ERR;
	}

	base_ptr = (char*)base_adr[bus];
	/* Set the I2C-device the user wants to use */
	dev = dev_find_slot(0, PCI_DEVFN(I2C1_DEV, bus + 1));

	/* Ensure we have the right PCI device */
	if ((pci_read_config16(dev, 0x0) != I2C_PCI_VENDOR_ID) ||
	    (pci_read_config16(dev, 0x2) != (I2C0_PCI_DEV_ID + bus))) {
		printk(BIOS_ERR, "I2C: Controller %d not found!\n", bus);
		return I2C_ERR;
	}

	/* Set memory base */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, (int)base_ptr);

	/* Enable memory space */
	pci_write_config32(dev, PCI_COMMAND,
			   (pci_read_config32(dev, PCI_COMMAND) | 0x2));

	/* Set up some settings of I2C controller */
	write32(base_ptr + I2C_CTRL,
		I2C_RESTART_EN | (I2C_STANDARD_MODE << 1) | I2C_MASTER_ENABLE);
	/* Adjust frequency for standard mode to 100 kHz */
	/* The counter value can be computed by N=100MHz/2/I2C_CLK */
	/* Thus, for 100 kHz I2C_CLK, N is 0x1F4 */
	write32(base_ptr + I2C_SS_SCL_HCNT, 0x1f4);
	write32(base_ptr + I2C_SS_SCL_LCNT, 0x1f4);
	/* For 400 kHz, the counter value is 0x7d */
	write32(base_ptr + I2C_FS_SCL_HCNT, 0x7d);
	write32(base_ptr + I2C_FS_SCL_LCNT, 0x7d);
	/* no interrupts in BIOS */
	write32(base_ptr + I2C_INTR_MASK, 0);

	/* Enable the I2C controller for operation */
	write32(base_ptr + I2C_ENABLE, 0x1);

	printk(BIOS_INFO, "I2C: Controller %d enabled.\n", bus);
	return I2C_SUCCESS;
}

/** \brief Read bytes over I2C-Bus from a slave. This function tries only one
 *         time to transmit data. In case of an error (abort) error code is
 *         returned. Retransmission has to be done from caller!
 * @param bus  Number of the I2C-controller to use (0...6)
 * @param chip 7 Bit of the slave address on I2C bus
 * @param addr Address inside slave where to read from
 * @param *buf Pointer to the buffer where to store read data
 * @param len  Number of bytes to read
 * @return     I2C_SUCCESS when read was successful, otherwise error code
 */
int i2c_read(unsigned bus, unsigned chip, unsigned addr,
			uint8_t *buf, unsigned len)
{
	int i = 0;
	char *base_ptr = NULL;
	struct device *dev;
	unsigned int val;
	int stat;

	/* Get base address of desired I2C-controller */
	dev = dev_find_slot(0, PCI_DEVFN(I2C1_DEV, bus + 1));
	base_ptr = (char *)pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	if (base_ptr == NULL) {
		printk(BIOS_INFO, "I2C: Invalid Base address\n");
		return I2C_ERR_INVALID_ADR;
	}

	/* Ensure I2C controller is not active before setting slave address */
	stat = wait_for_idle(base_ptr);
	if (stat != I2C_SUCCESS)
		return stat;

	/* clear any abort status from a previous transaction */
	read32(base_ptr + I2C_CLR_TX_ABRT);

	/* Now we can program the desired slave address and start transfer */
	write32(base_ptr + I2C_TARGET_ADR, chip & 0xff);
	/* Send address inside slave to read from */
	write32(base_ptr + I2C_DATA_CMD, addr & 0xff);

	/* For the next byte we need a repeated start condition */
	val = I2C_RW_CMD | I2C_RESTART;
	/* Now we can read desired amount of data over I2C */
	for (i = 0; i < len; i++) {
		/* A read is initiated by writing dummy data to the DATA-register */
		write32(base_ptr + I2C_DATA_CMD, val);
		stat = wait_rx_fifo(base_ptr);
		if (stat)
			return stat;
		buf[i] = read32(base_ptr + I2C_DATA_CMD) & 0xff;
		val = I2C_RW_CMD;
		if (i == (len - 2)) {
			/* For the last byte we need a stop condition to be generated */
			val |= I2C_STOP;
		}
	}
	return I2C_SUCCESS;
}

/** \brief Write bytes over I2C-Bus from a slave. This function tries only one
 *         time to transmit data. In case of an error (abort) error code is
 *         returned. Retransmission has to be done from caller!
 * @param bus  Number of the I2C-controller to use (0...6)
 * @param chip 7 Bit of the slave address on I2C bus
 * @param addr Address inside slave where to write to
 * @param *buf Pointer to the buffer where data to write is stored
 * @param len  Number of bytes to write
 * @return     I2C_SUCCESS when read was successful, otherwise error code
 */
int i2c_write(unsigned bus, unsigned chip, unsigned addr,
			const uint8_t *buf, unsigned len)
{
	int i;
	char *base_ptr;
	struct device *dev;
	unsigned int val;
	int stat;

	/* Get base address of desired I2C-controller */
	dev = dev_find_slot(0, PCI_DEVFN(I2C1_DEV, bus + 1));
	base_ptr = (char *)pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	if (base_ptr == NULL) {
		return I2C_ERR_INVALID_ADR;
	}

	/* Ensure I2C controller is not active yet */
	stat = wait_for_idle(base_ptr);
	if (stat) {
		return stat;
	}

	/* clear any abort status from a previous transaction */
	read32(base_ptr + I2C_CLR_TX_ABRT);

	/* Program slave address to use for this transfer */
	write32(base_ptr + I2C_TARGET_ADR, chip & 0xff);

	/* Send address inside slave to write data to */
	write32(base_ptr + I2C_DATA_CMD, addr & 0xff);

	for (i = 0; i < len; i++) {
		val = (unsigned int)(buf[i] & 0xff);	/* Take only 8 bits */
		if (i == (len - 1)) {
			/* For the last byte we need a stop condition */
			val |= I2C_STOP;
		}
		stat = wait_tx_fifo(base_ptr);
		if (stat) {
			return stat;
		}
		write32(base_ptr + I2C_DATA_CMD, val);
	}
	return I2C_SUCCESS;
}
