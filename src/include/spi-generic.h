/*
 * (C) Copyright 2001
 * Gerald Van Baren, Custom IDEAS, vanbaren@cideas.com.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SPI_GENERIC_H_
#define _SPI_GENERIC_H_

#include <stdint.h>
#include <stddef.h>

/* Controller-specific definitions: */

struct spi_ctrlr;

/*-----------------------------------------------------------------------
 * Representation of a SPI slave, i.e. what we're communicating with.
 *
 *   bus:	ID of the bus that the slave is attached to.
 *   cs:	ID of the chip select connected to the slave.
 *   ctrlr:	Pointer to SPI controller structure.
 */
struct spi_slave {
	unsigned int	bus;
	unsigned int	cs;
	const struct spi_ctrlr *ctrlr;
};

/* Representation of SPI operation status. */
enum spi_op_status {
	SPI_OP_NOT_EXECUTED = 0,
	SPI_OP_SUCCESS = 1,
	SPI_OP_FAILURE = 2,
};

/*
 * Representation of a SPI operation.
 *
 * dout:	Pointer to data to send.
 * bytesout:	Count of data in bytes to send.
 * din:	Pointer to store received data.
 * bytesin:	Count of data in bytes to receive.
 */
struct spi_op {
	const void *dout;
	size_t bytesout;
	void *din;
	size_t bytesin;
	enum spi_op_status status;
};

enum spi_clock_phase {
	SPI_CLOCK_PHASE_FIRST,
	SPI_CLOCK_PHASE_SECOND
};

enum spi_wire_mode {
	SPI_4_WIRE_MODE,
	SPI_3_WIRE_MODE
};

enum spi_polarity {
	SPI_POLARITY_LOW,
	SPI_POLARITY_HIGH
};

struct spi_cfg {
	/* CLK phase - 0: Phase first, 1: Phase second */
	enum spi_clock_phase clk_phase;
	/* CLK polarity - 0: Low, 1: High */
	enum spi_polarity clk_polarity;
	/* CS polarity - 0: Low, 1: High */
	enum spi_polarity cs_polarity;
	/* Wire mode - 0: 4-wire, 1: 3-wire */
	enum spi_wire_mode wire_mode;
	/* Data bit length. */
	unsigned int data_bit_length;
};

/*-----------------------------------------------------------------------
 * Representation of a SPI contoller.
 *
 * get_config:	Get configuration of SPI bus
 * claim_bus:	Claim SPI bus and prepare for communication.
 * release_bus: Release SPI bus.
 * setup:	Setup given SPI device bus.
 * xfer:	Perform one SPI transfer operation.
 * xfer_vector: Vector of SPI transfer operations.
 */
struct spi_ctrlr {
	int (*get_config)(const struct spi_slave *slave,
			struct spi_cfg *cfg);
	int (*claim_bus)(const struct spi_slave *slave);
	void (*release_bus)(const struct spi_slave *slave);
	int (*setup)(const struct spi_slave *slave);
	int (*xfer)(const struct spi_slave *slave, const void *dout,
		    size_t bytesout, void *din, size_t bytesin);
	int (*xfer_vector)(const struct spi_slave *slave,
			struct spi_op vectors[], size_t count);
};

/*-----------------------------------------------------------------------
 * Structure defining mapping of SPI buses to controller.
 *
 * ctrlr:	Pointer to controller structure managing the given SPI buses.
 * bus_start:	Start bus number managed by the controller.
 * bus_end:	End bus number manager by the controller.
 */
struct spi_ctrlr_buses {
	const struct spi_ctrlr *ctrlr;
	unsigned int bus_start;
	unsigned int bus_end;
};

/* Mapping of SPI buses to controllers - should be defined by platform. */
extern const struct spi_ctrlr_buses spi_ctrlr_bus_map[];
extern const size_t spi_ctrlr_bus_map_count;

/*-----------------------------------------------------------------------
 * Initialization, must be called once on start up.
 *
 */
void spi_init(void);

/*
 * Get configuration of SPI bus.
 *
 * slave:     Pointer to slave structure.
 * cfg:       Pointer to SPI configuration that needs to be filled.
 *
 * Returns:
 * 0 on success, -1 on error
 */
int spi_get_config(const struct spi_slave *slave, struct spi_cfg *cfg);

/*-----------------------------------------------------------------------
 * Set up communications parameters for a SPI slave.
 *
 * This must be called once for each slave. Note that this function
 * usually doesn't touch any actual hardware, it only initializes the
 * contents of spi_slave so that the hardware can be easily
 * initialized later.
 *
 *   bus:     Bus ID of the slave chip.
 *   cs:      Chip select ID of the slave chip on the specified bus.
 *   slave:   Pointer to slave structure that needs to be initialized.
 *
 * Returns:
 * 0 on success, -1 on error
 */
int spi_setup_slave(unsigned int bus, unsigned int cs, struct spi_slave *slave);

/*-----------------------------------------------------------------------
 * Claim the bus and prepare it for communication with a given slave.
 *
 * This must be called before doing any transfers with a SPI slave. It
 * will enable and initialize any SPI hardware as necessary, and make
 * sure that the SCK line is in the correct idle state. It is not
 * allowed to claim the same bus for several slaves without releasing
 * the bus in between.
 *
 *   slave:	The SPI slave
 *
 * Returns: 0 if the bus was claimed successfully, or a negative value
 * if it wasn't.
 */
int spi_claim_bus(const struct spi_slave *slave);

/*-----------------------------------------------------------------------
 * Release the SPI bus
 *
 * This must be called once for every call to spi_claim_bus() after
 * all transfers have finished. It may disable any SPI hardware as
 * appropriate.
 *
 *   slave:	The SPI slave
 */
void spi_release_bus(const struct spi_slave *slave);

/*-----------------------------------------------------------------------
 * SPI transfer
 *
 * spi_xfer() interface:
 *   slave:	The SPI slave which will be sending/receiving the data.
 *   dout:	Pointer to a string of bytes to send out.
 *   bytesout:	How many bytes to write.
 *   din:	Pointer to a string of bytes that will be filled in.
 *   bytesin:	How many bytes to read.
 *
 *   Returns: 0 on success, not 0 on failure
 */
int spi_xfer(const struct spi_slave *slave, const void *dout, size_t bytesout,
	     void *din, size_t bytesin);

/*-----------------------------------------------------------------------
 * Vector of SPI transfer operations
 *
 * spi_xfer_vector() interface:
 *   slave:	The SPI slave which will be sending/receiving the data.
 *   vectors:	Array of SPI op structures.
 *   count:	Number of SPI op vectors.
 *
 *   Returns: 0 on success, not 0 on failure
 */
int spi_xfer_vector(const struct spi_slave *slave,
		struct spi_op vectors[], size_t count);

unsigned int spi_crop_chunk(unsigned int cmd_len, unsigned int buf_len);

/*-----------------------------------------------------------------------
 * Write 8 bits, then read 8 bits.
 *   slave:	The SPI slave we're communicating with
 *   byte:	Byte to be written
 *
 * Returns: The value that was read, or a negative value on error.
 *
 * TODO: This function probably shouldn't be inlined.
 */
static inline int spi_w8r8(const struct spi_slave *slave, unsigned char byte)
{
	unsigned char dout[2];
	unsigned char din[2];
	int ret;

	dout[0] = byte;
	dout[1] = 0;

	ret = spi_xfer(slave, dout, 2, din, 2);
	return ret < 0 ? ret : din[1];
}

/*
 * Helper function to allow chipsets to combine two vectors if possible. It can
 * only handle upto 2 vectors.
 *
 * This function is provided to support command-response kind of transactions
 * expected by users like flash. Some special SPI flash controllers can handle
 * such command-response operations in a single transaction. For these special
 * controllers, separate command and response vectors can be combined into a
 * single operation.
 *
 * Two vectors are combined if first vector has a non-NULL dout and NULL din and
 * second vector has a non-NULL din and NULL dout. Otherwise, each vector is
 * operated upon one at a time.
 *
 * Returns 0 on success and non-zero on failure.
 */
int spi_xfer_two_vectors(const struct spi_slave *slave,
			struct spi_op vectors[], size_t count);

#endif	/* _SPI_GENERIC_H_ */
