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

#include <commonlib/region.h>
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

/*
 * If there is no limit on the maximum transfer size for the controller,
 * max_xfer_size can be set to SPI_CTRLR_DEFAULT_MAX_XFER_SIZE which is equal to
 * UINT32_MAX.
 */
#define SPI_CTRLR_DEFAULT_MAX_XFER_SIZE	(UINT32_MAX)

struct spi_flash;

enum {
	/* Deduct the command length from the spi_crop_chunk() calculation for
	   sizing a transaction. */
	SPI_CNTRLR_DEDUCT_CMD_LEN = 1 << 0,
	/* Remove the opcode size from the command length used in the
	   spi_crop_chunk() calculation. Controllers which have a dedicated
	   register for the command byte would set this flag which would
	   allow the use of the maximum transfer size. */
	SPI_CNTRLR_DEDUCT_OPCODE_LEN = 1 << 1,
};

/*-----------------------------------------------------------------------
 * Representation of a SPI controller. Note the xfer() and xfer_vector()
 * callbacks are meant to process full duplex transactions. If the
 * controller cannot handle these transactions then return an error when
 * din and dout are both set. See spi_xfer() below for more details.
 *
 * claim_bus:		Claim SPI bus and prepare for communication.
 * release_bus:	Release SPI bus.
 * setup:		Setup given SPI device bus.
 * xfer:		Perform one SPI transfer operation.
 * xfer_vector:	Vector of SPI transfer operations.
 * max_xfer_size:	Maximum transfer size supported by the controller
 *			(0 = invalid,
 *			 SPI_CTRLR_DEFAULT_MAX_XFER_SIZE = unlimited)
 * flags:		See SPI_CNTRLR_* enums above.
 *
 * Following member is provided by specialized SPI controllers that are
 * actually SPI flash controllers.
 *
 * flash_probe:	Specialized probe function provided by SPI flash
 *			controllers.
 * flash_protect: Protect a region of flash using the SPI flash controller.
 */
struct spi_ctrlr {
	int (*claim_bus)(const struct spi_slave *slave);
	void (*release_bus)(const struct spi_slave *slave);
	int (*setup)(const struct spi_slave *slave);
	int (*xfer)(const struct spi_slave *slave, const void *dout,
		    size_t bytesout, void *din, size_t bytesin);
	int (*xfer_vector)(const struct spi_slave *slave,
			struct spi_op vectors[], size_t count);
	uint32_t max_xfer_size;
	uint32_t flags;
	int (*flash_probe)(const struct spi_slave *slave,
				struct spi_flash *flash);
	int (*flash_protect)(const struct spi_flash *flash,
				const struct region *region);
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
 * Note that din and dout are transferred simulataneously in a full duplex
 * transaction. The number of clocks within one transaction is calculated
 * as: MAX(bytesout*8, bytesin*8).
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

/*-----------------------------------------------------------------------
 * Given command length and length of remaining data, return the maximum data
 * that can be transferred in next spi_xfer.
 *
 * Returns: 0 on error, non-zero data size that can be xfered on success.
 */
unsigned int spi_crop_chunk(const struct spi_slave *slave, unsigned int cmd_len,
			unsigned int buf_len);

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

#endif	/* _SPI_GENERIC_H_ */
