/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
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

#include <console/console.h>
#include <soc/cpu.h>
#include <soc/spi.h>
#include <spi_flash.h>
#include <spi-generic.h>
#include <stdlib.h>
#include <string.h>
#include <timer.h>

/* Imgtec controller uses 16 bit packet length. */
#define IMGTEC_SPI_MAX_TRANSFER_SIZE   ((1 << 16) - 1)

struct img_spi_slave {
	/* SPIM instance device parameters */
	struct spim_device_parameters device_parameters;
	/* SPIM instance base address */
	u32 base;
	/* Boolean property that is TRUE if API has been initialised */
	int initialised;
};

/* Allocate memory for the maximum number of devices */
static struct
img_spi_slave img_spi_slaves[SPIM_NUM_BLOCKS*SPIM_NUM_PORTS_PER_BLOCK];

/*
 * Wait for the bit at the shift position to be set in reg
 * If the bit is not set in SPI_TIMEOUT_VALUE_US return with error
 */
static int wait_status(u32 reg, u32 shift)
{
	struct stopwatch sw;

	stopwatch_init_usecs_expire(&sw, SPI_TIMEOUT_VALUE_US);
	while (!(read32(reg) & (1 << shift))) {
		if (stopwatch_expired(&sw))
			return -SPIM_TIMEOUT;
	}
	return SPIM_OK;
}

static struct img_spi_slave *get_img_slave(const struct spi_slave *slave)
{
	return img_spi_slaves + slave->bus * SPIM_NUM_PORTS_PER_BLOCK +
		slave->cs;
}

/* Transmitter function. Fills TX FIFO with data before enabling SPIM */
static int transmitdata(const struct spi_slave *slave, u8 *buffer, u32 size)
{
	u32 blocksize, base, write_data;
	int ret;
	struct img_spi_slave *img_slave = get_img_slave(slave);

	base = img_slave->base;
	while (size) {
		/* Wait until FIFO empty */
		write32(base + SPFI_INT_CLEAR_REG_OFFSET, SPFI_SDE_MASK);
		ret = wait_status(base + SPFI_INT_STATUS_REG_OFFSET,
					SPFI_SDE_SHIFT);
		if (ret)
			return ret;

		/*
		 * Write to FIFO in blocks of 16 words (64 bytes)
		 * Do 32bit writes first.
		 */
		blocksize = SPIM_MAX_BLOCK_BYTES;
		while ((size >= sizeof(u32)) && blocksize) {
			memcpy(&write_data, buffer, sizeof(u32));
			write32(base + SPFI_SEND_LONG_REG_OFFSET, write_data);
			buffer += sizeof(u32);
			size -= sizeof(u32);
			blocksize -= sizeof(u32);
		}
		while (size && blocksize) {
			write32(base + SPFI_SEND_BYTE_REG_OFFSET, *buffer);
			buffer++;
			size--;
			blocksize--;
		}
	}
	return SPIM_OK;
}

/* Receiver function */
static int receivedata(const struct spi_slave *slave, u8 *buffer, u32 size)
{
	u32 read_data, base;
	int ret;
	struct img_spi_slave *img_slave = get_img_slave(slave);

	base = img_slave->base;
	/*
	 * Do 32bit reads first. Clear status GDEX32BIT here so that the first
	 * status reg. read gets the actual bit state
	 */
	write32(base + SPFI_INT_CLEAR_REG_OFFSET, SPFI_GDEX32BIT_MASK);
	while (size >= sizeof(u32)) {
		ret = wait_status(base + SPFI_INT_STATUS_REG_OFFSET,
					SPFI_GDEX32BIT_SHIFT);
		if (ret)
			return ret;
		read_data = read32(base + SPFI_GET_LONG_REG_OFFSET);
		memcpy(buffer, &read_data, sizeof(u32));
		buffer += sizeof(u32);
		size -= sizeof(u32);
		/* Clear interrupt status on GDEX32BITL */
		write32(base + SPFI_INT_CLEAR_REG_OFFSET, SPFI_GDEX32BIT_MASK);
	}

	/*
	 * Do the remaining 8bit reads. Clear status GDEX8BIT here so that
	 * the first status reg. read gets the actual bit state
	 */
	write32(base + SPFI_INT_CLEAR_REG_OFFSET, SPFI_GDEX8BIT_MASK);
	while (size) {
		ret = wait_status(base + SPFI_INT_STATUS_REG_OFFSET,
					SPFI_GDEX8BIT_SHIFT);
		if (ret)
			return ret;
		*buffer = read32(base + SPFI_GET_BYTE_REG_OFFSET);
		buffer++;
		size--;
		/* Clear interrupt status on SPFI_GDEX8BIT */
		write32(base + SPFI_INT_CLEAR_REG_OFFSET, SPFI_GDEX8BIT_MASK);
	}
	return SPIM_OK;
}

/* Sets port parameters in port state register. */
static void  setparams(const struct spi_slave *slave, u32 port,
			struct spim_device_parameters *params)
{
	u32 spim_parameters, port_state, base;
	struct img_spi_slave *img_slave = get_img_slave(slave);

	base = img_slave->base;
	spim_parameters = 0;
	port_state = read32(base + SPFI_PORT_STATE_REG_OFFSET);
	port_state &= ~((SPIM_PORT0_MASK>>port)|SPFI_PORT_SELECT_MASK);
	port_state |= params->cs_idle_level<<(SPIM_CS0_IDLE_SHIFT-port);
	port_state |=
		params->data_idle_level<<(SPIM_DATA0_IDLE_SHIFT-port);

	/* Clock idle level and phase */
	switch (params->spi_mode) {
	case SPIM_MODE_0:
		break;
	case SPIM_MODE_1:
		port_state |= (1 << (SPIM_CLOCK0_PHASE_SHIFT - port));
		break;
	case SPIM_MODE_2:
		port_state |= (1 << (SPIM_CLOCK0_IDLE_SHIFT - port));
		break;
	case SPIM_MODE_3:
		port_state |= (1 << (SPIM_CLOCK0_IDLE_SHIFT - port)) |
				 (1 << (SPIM_CLOCK0_PHASE_SHIFT - port));
		break;
	}
	/* Set port state register */
	write32(base + SPFI_PORT_STATE_REG_OFFSET, port_state);

	/* Set up values to be written to device parameter register */
	spim_parameters |= params->bitrate << SPIM_CLK_DIVIDE_SHIFT;
	spim_parameters |= params->cs_setup << SPIM_CS_SETUP_SHIFT;
	spim_parameters |= params->cs_hold << SPIM_CS_HOLD_SHIFT;
	spim_parameters |= params->cs_delay << SPIM_CS_DELAY_SHIFT;

	write32(base + SPFI_PORT_0_PARAM_REG_OFFSET + 4 * port,
			spim_parameters);
}

/* Sets up transaction register */
static u32 transaction_reg_setup(struct spim_buffer *first,
					struct spim_buffer *second)
{
	u32 reg = 0;

	/* 2nd transfer exists? */
	if (second) {
		/*
		 * If second transfer exists, it's a "command followed by data"
		 * type of transfer and first transfer is defined by
		 * CMD_LENGTH, ADDR_LENGTH, DUMMY_LENGTH... fields of
		 * transaction register
		 */
		reg = spi_write_reg_field(reg, SPFI_CMD_LENGTH, 1);
		reg = spi_write_reg_field(reg, SPFI_ADDR_LENGTH,
						first->size - 1);
		reg = spi_write_reg_field(reg, SPFI_DUMMY_LENGTH, 0);
		/* Set data size (size of the second transfer) */
		reg = spi_write_reg_field(reg, SPFI_TSIZE, second->size);
	} else {
		/* Set data size, in this case size of the 1st transfer */
		reg = spi_write_reg_field(reg, SPFI_TSIZE, first->size);
	}
	return reg;
}

/* Sets up control register */
static u32 control_reg_setup(struct spim_buffer *first,
				struct spim_buffer *second)
{
	u32 reg;

	/* Enable SPFI */
	reg = SPFI_EN_MASK;
	reg |= first->inter_byte_delay ? SPIM_BYTE_DELAY_MASK : 0;

	/* Set up the transfer mode */
	reg = spi_write_reg_field(reg, SPFI_TRNSFR_MODE_DQ, SPIM_CMD_MODE_0);
	reg = spi_write_reg_field(reg, SPFI_TRNSFR_MODE, SPIM_DMODE_SINGLE);
	reg = spi_write_reg_field(reg, SPIM_EDGE_TX_RX, 1);

	if (second) {
		/* Set TX bit if the 2nd transaction is 'send' */
		reg = spi_write_reg_field(reg, SPFI_TX_RX,
						second->isread ? 0 : 1);
		/*
		 * Set send/get DMA for both transactions
		 * (first is always 'send')
		 */
		reg = spi_write_reg_field(reg, SPIM_SEND_DMA, 1);
		if (second->isread)
			reg = spi_write_reg_field(reg, SPIM_GET_DMA, 1);

	} else {
		/* Set TX bit if the 1st transaction is 'send' */
		reg |= first->isread ? 0 : SPFI_TX_RX_MASK;
		/* Set send/get DMA */
		reg |= first->isread ? SPIM_GET_DMA_MASK : SPIM_SEND_DMA_MASK;
	}
	return reg;
}

/* Checks the given buffer information */
static int check_buffers(const struct spi_slave *slave, struct spim_buffer *first,
				struct spim_buffer *second){

	struct img_spi_slave *img_slave = get_img_slave(slave);

	if (!(img_slave->initialised))
		return -SPIM_API_NOT_INITIALISED;
	/*
	 * First operation must always be defined
	 * It can be either a read or a write and its size cannot be bigge
	 * than SPIM_MAX_TANSFER_BYTES = 64KB - 1 (0xFFFF)
	 */
	if (!first)
		return -SPIM_INVALID_READ_WRITE;
	if (first->size > SPIM_MAX_TRANSFER_BYTES)
		return -SPIM_INVALID_SIZE;
	if (first->isread > 1)
		return -SPIM_INVALID_READ_WRITE;
	/* Check operation parameters for 'second' */
	if (second) {
		/*
		 * If the second operation is defined it must be a read
		 * operation and its size must not be bigger than
		 * SPIM_MAX_TANSFER_BYTES = 64KB - 1 (0xFFFF)
		 */
		if (second->size > SPIM_MAX_TRANSFER_BYTES)
			return -SPIM_INVALID_SIZE;
		if (!second->isread)
			return -SPIM_INVALID_READ_WRITE;
		/*
		 * If the second operations is defined, the first operation
		 * must be a write and its size cannot be bigger than
		 * SPIM_MAX_FLASH_COMMAND_BYTES(8): command size (1) +
		 * address size (7).
		 */
		if (first->isread)
			return -SPIM_INVALID_READ_WRITE;
		if (first->size > SPIM_MAX_FLASH_COMMAND_BYTES)
			return -SPIM_INVALID_SIZE;

	}
	return SPIM_OK;
}

/* Checks the set bitrate */
static int check_bitrate(u32 rate)
{
	/* Bitrate must be 1, 2, 4, 8, 16, 32, 64, or 128 */
	switch (rate) {
	case 1:
	case 2:
	case 4:
	case 8:
	case 16:
	case 32:
	case 64:
	case 128:
		return SPIM_OK;
	default:
		return -SPIM_INVALID_BIT_RATE;
	}
	return -SPIM_INVALID_BIT_RATE;
}

/* Checks device parameters for errors */
static int check_device_params(struct spim_device_parameters *pdev_param)
{
	if (pdev_param->spi_mode < SPIM_MODE_0 ||
		pdev_param->spi_mode > SPIM_MODE_3)
		return -SPIM_INVALID_SPI_MODE;
	if (check_bitrate(pdev_param->bitrate) != SPIM_OK)
		return -SPIM_INVALID_BIT_RATE;
	if (pdev_param->cs_idle_level > 1)
		return -SPIM_INVALID_CS_IDLE_LEVEL;
	if (pdev_param->data_idle_level > 1)
		return -SPIM_INVALID_DATA_IDLE_LEVEL;
	return SPIM_OK;
}

/* Function that carries out read/write operations */
static int spim_io(const struct spi_slave *slave, struct spim_buffer *first,
			struct spim_buffer *second)
{
	u32 reg, base;
	int i, trans_count, ret;
	struct spim_buffer *transaction[2];
	struct img_spi_slave *img_slave = get_img_slave(slave);

	base = img_slave->base;

	ret = check_buffers(slave, first, second);
	if (ret)
		return ret;

	/*
	 * Soft reset peripheral internals, this will terminate any
	 * pending transactions
	 */
	write32(base + SPFI_CONTROL_REG_OFFSET, SPIM_SOFT_RESET_MASK);
	write32(base + SPFI_CONTROL_REG_OFFSET, 0);
	/* Port state register */
	reg = read32(base +  SPFI_PORT_STATE_REG_OFFSET);
	reg = spi_write_reg_field(reg, SPFI_PORT_SELECT, slave->cs);
	write32(base + SPFI_PORT_STATE_REG_OFFSET, reg);
	/* Set transaction register */
	reg = transaction_reg_setup(first, second);
	write32(base + SPFI_TRANSACTION_REG_OFFSET, reg);
	/* Clear status */
	write32(base + SPFI_INT_CLEAR_REG_OFFSET, 0xffffffff);
	/* Set control register */
	reg = control_reg_setup(first, second);
	write32(base + SPFI_CONTROL_REG_OFFSET, reg);
	/* First transaction always exists */
	transaction[0] = first;
	trans_count = 1;
	/* Is there a second transaction? */
	if (second) {
		transaction[1] = second;
		trans_count++;
	}
	/* Now write/read FIFO's */
	for (i = 0; i < trans_count; i++)
		/* Which transaction to execute, "Send" or "Get"? */
		if (transaction[i]->isread) {
			/* Get */
			ret = receivedata(slave, transaction[i]->buffer,
					transaction[i]->size);
			if (ret) {
				printk(BIOS_ERR,
					"%s: Error: receive data failed.\n",
					__func__);
				return ret;
			}
		} else {
			/* Send */
			ret = transmitdata(slave, transaction[i]->buffer,
					transaction[i]->size);
			if (ret) {
				printk(BIOS_ERR,
					"%s: Error: transmit data failed.\n",
					__func__);
				return ret;
			}
		}

	/* Wait for end of the transaction */
	ret = wait_status(base + SPFI_INT_STATUS_REG_OFFSET,
				SPFI_ALLDONE_SHIFT);
	if (ret)
		return ret;
	/*
	 * Soft reset peripheral internals, this will terminate any
	 * pending transactions
	 */
	write32(base + SPFI_CONTROL_REG_OFFSET, SPIM_SOFT_RESET_MASK);
	write32(base + SPFI_CONTROL_REG_OFFSET, 0);

	return SPIM_OK;
}

/* Claim the bus and prepare it for communication */
static int spi_ctrlr_claim_bus(const struct spi_slave *slave)
{
	int ret;
	struct img_spi_slave *img_slave;

	if (!slave) {
		printk(BIOS_ERR, "%s: Error: slave was not set up.\n",
				__func__);
		return -SPIM_API_NOT_INITIALISED;
	}
	img_slave = get_img_slave(slave);
	if (img_slave->initialised)
		return SPIM_OK;
	/* Check device parameters */
	ret = check_device_params(&(img_slave->device_parameters));
	if (ret) {
		printk(BIOS_ERR, "%s: Error: incorrect device parameters.\n",
				__func__);
		return ret;
	}
	/* Set device parameters */
	setparams(slave, slave->cs, &(img_slave->device_parameters));
	/* Soft reset peripheral internals */
	write32(img_slave->base + SPFI_CONTROL_REG_OFFSET,
		SPIM_SOFT_RESET_MASK);
	write32(img_slave->base + SPFI_CONTROL_REG_OFFSET, 0);
	img_slave->initialised = IMG_TRUE;
	return SPIM_OK;
}

/* Release the SPI bus */
static void spi_ctrlr_release_bus(const struct spi_slave *slave)
{
	struct img_spi_slave *img_slave;

	if (!slave) {
		printk(BIOS_ERR, "%s: Error: slave was not set up.\n",
				__func__);
		return;
	}
	img_slave = get_img_slave(slave);
	img_slave->initialised = IMG_FALSE;
	/* Soft reset peripheral internals */
	write32(img_slave->base + SPFI_CONTROL_REG_OFFSET,
		SPIM_SOFT_RESET_MASK);
	write32(img_slave->base + SPFI_CONTROL_REG_OFFSET, 0);
}

/* SPI transfer */
static int do_spi_xfer(const struct spi_slave *slave, const void *dout,
		       size_t bytesout, void *din, size_t bytesin)
{
	struct spim_buffer	buff_0;
	struct spim_buffer	buff_1;

	/* If we only have a read or a write operation
	 * the parameters for it will be put in the first buffer
	 */
	buff_0.buffer = (dout) ? (void *)dout : (void *)din;
	buff_0.size = (dout) ? bytesout : bytesin;
	buff_0.isread = (dout) ? IMG_FALSE : IMG_TRUE;
	buff_0.inter_byte_delay = 0;

	if (dout && din) {
		/* Set up the read buffer to receive our data */
		buff_1.buffer = din;
		buff_1.size = bytesin;
		buff_1.isread = IMG_TRUE;
		buff_1.inter_byte_delay = 0;
	}
	return spim_io(slave, &buff_0, (dout && din) ? &buff_1 : NULL);
}

static int spi_ctrlr_xfer(const struct spi_slave *slave, const void *dout,
			  size_t bytesout, void *din, size_t bytesin)
{
	unsigned int in_sz, out_sz;
	int ret;

	if (!slave) {
		printk(BIOS_ERR, "%s: Error: slave was not set up.\n",
				__func__);
		return -SPIM_API_NOT_INITIALISED;
	}
	if (!dout && !din) {
		printk(BIOS_ERR, "%s: Error: both buffers are NULL.\n",
				__func__);
			return -SPIM_INVALID_TRANSFER_DESC;
	}

	while (bytesin || bytesout) {
		in_sz = min(IMGTEC_SPI_MAX_TRANSFER_SIZE, bytesin);
		out_sz = min(IMGTEC_SPI_MAX_TRANSFER_SIZE, bytesout);

		ret = do_spi_xfer(slave, dout, out_sz, din, in_sz);
		if (ret)
			return ret;

		bytesin -= in_sz;
		bytesout -= out_sz;

		if (bytesin)
			din += in_sz;
		else
			din = NULL;

		if (bytesout)
			dout += out_sz;
		else
			dout = NULL;
	}

	return SPIM_OK;
}

static int spi_ctrlr_setup(const struct spi_slave *slave)
{
	struct img_spi_slave *img_slave = NULL;
	struct spim_device_parameters *device_parameters;
	u32 base;

	switch (slave->bus) {
	case 0:
		base = IMG_SPIM0_BASE_ADDRESS;
		break;
	case 1:
		base = IMG_SPIM1_BASE_ADDRESS;
		break;
	default:
		printk(BIOS_ERR, "%s: Error: unsupported bus.\n",
				__func__);
		return -1;
	}
	if (slave->cs > SPIM_DEVICE4) {
		printk(BIOS_ERR, "%s: Error: unsupported chipselect.\n",
				__func__);
		return -1;
	}

	img_slave = get_img_slave(slave);
	device_parameters = &(img_slave->device_parameters);

	img_slave->base = base;

	device_parameters->bitrate = 64;
	device_parameters->cs_setup = 0;
	device_parameters->cs_hold = 0;
	device_parameters->cs_delay = 0;
	device_parameters->spi_mode = SPIM_MODE_0;
	device_parameters->cs_idle_level = 1;
	device_parameters->data_idle_level = 0;
	img_slave->initialised = IMG_FALSE;

	return 0;
}

static const struct spi_ctrlr spi_ctrlr = {
	.setup = spi_ctrlr_setup,
	.claim_bus = spi_ctrlr_claim_bus,
	.release_bus = spi_ctrlr_release_bus,
	.xfer = spi_ctrlr_xfer,
	.max_xfer_size = IMGTEC_SPI_MAX_TRANSFER_SIZE,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = 0,
		.bus_end = 1,
	},
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
