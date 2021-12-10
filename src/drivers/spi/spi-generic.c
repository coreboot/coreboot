/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <commonlib/helpers.h>
#include <spi-generic.h>
#include <stddef.h>
#include <string.h>

int spi_claim_bus(const struct spi_slave *slave)
{
	const struct spi_ctrlr *ctrlr = slave->ctrlr;
	if (ctrlr && ctrlr->claim_bus)
		return ctrlr->claim_bus(slave);
	return 0;
}

void spi_release_bus(const struct spi_slave *slave)
{
	const struct spi_ctrlr *ctrlr = slave->ctrlr;
	if (ctrlr && ctrlr->release_bus)
		ctrlr->release_bus(slave);
}

static int spi_xfer_single_op(const struct spi_slave *slave,
			struct spi_op *op)
{
	const struct spi_ctrlr *ctrlr = slave->ctrlr;
	int ret;

	if (!ctrlr || !ctrlr->xfer)
		return -1;

	ret = ctrlr->xfer(slave, op->dout, op->bytesout, op->din, op->bytesin);
	if (ret)
		op->status = SPI_OP_FAILURE;
	else
		op->status = SPI_OP_SUCCESS;

	return ret;
}

static int spi_xfer_vector_default(const struct spi_slave *slave,
				struct spi_op vectors[], size_t count)
{
	size_t i;
	int ret;

	for (i = 0; i < count; i++) {
		ret = spi_xfer_single_op(slave, &vectors[i]);
		if (ret)
			return ret;
	}

	return 0;
}

int spi_xfer_vector(const struct spi_slave *slave,
		struct spi_op vectors[], size_t count)
{
	const struct spi_ctrlr *ctrlr = slave->ctrlr;

	if (ctrlr && ctrlr->xfer_vector)
		return ctrlr->xfer_vector(slave, vectors, count);

	return spi_xfer_vector_default(slave, vectors, count);
}

int spi_xfer(const struct spi_slave *slave, const void *dout, size_t bytesout,
	     void *din, size_t bytesin)
{
	const struct spi_ctrlr *ctrlr = slave->ctrlr;

	if (ctrlr && ctrlr->xfer)
		return ctrlr->xfer(slave, dout, bytesout, din, bytesin);

	return -1;
}

unsigned int spi_crop_chunk(const struct spi_slave *slave, unsigned int cmd_len,
			unsigned int buf_len)
{
	const struct spi_ctrlr *ctrlr = slave->ctrlr;
	unsigned int ctrlr_max;
	bool deduct_cmd_len;
	bool deduct_opcode_len;

	if (!ctrlr)
		return 0;

	deduct_cmd_len = !!(ctrlr->flags & SPI_CNTRLR_DEDUCT_CMD_LEN);
	deduct_opcode_len = !!(ctrlr->flags & SPI_CNTRLR_DEDUCT_OPCODE_LEN);
	ctrlr_max = ctrlr->max_xfer_size;

	assert (ctrlr_max != 0);

	/* Assume opcode is always one byte and deduct it from the cmd_len
	   as the hardware has a separate register for the opcode. */
	if (deduct_opcode_len)
		cmd_len--;

	/* Subtract command length from useable buffer size. If
	   deduct_opcode_len is set, only subtract the number command bytes
	   after the opcode. If the adjusted cmd_len is larger than ctrlr_max
	   return 0 to inidicate an error. */
	if (deduct_cmd_len) {
		if (ctrlr_max >= cmd_len) {
			ctrlr_max -= cmd_len;
		} else {
			ctrlr_max = 0;
			printk(BIOS_WARNING, "%s: Command longer than buffer\n", __func__);
		}
	}

	return MIN(ctrlr_max, buf_len);
}

void __weak spi_init(void)
{
	/* Default weak implementation - do nothing. */
}

int spi_setup_slave(unsigned int bus, unsigned int cs, struct spi_slave *slave)
{
	size_t i;

	memset(slave, 0, sizeof(*slave));

	for (i = 0; i < spi_ctrlr_bus_map_count; i++) {
		if ((spi_ctrlr_bus_map[i].bus_start <= bus) &&
		    (spi_ctrlr_bus_map[i].bus_end >= bus)) {
			slave->ctrlr = spi_ctrlr_bus_map[i].ctrlr;
			break;
		}
	}

	if (slave->ctrlr == NULL) {
		printk(BIOS_ERR, "Can't find SPI bus %u\n", bus);
		return -1;
	}

	slave->bus = bus;
	slave->cs = cs;

	if (slave->ctrlr->setup)
		return slave->ctrlr->setup(slave);

	return 0;
}
