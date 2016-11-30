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

/*-----------------------------------------------------------------------
 * Representation of a SPI slave, i.e. what we're communicating with.
 *
 *   bus:	ID of the bus that the slave is attached to.
 *   cs:	ID of the chip select connected to the slave.
 */
struct spi_slave {
	unsigned int	bus;
	unsigned int	cs;
};

/*-----------------------------------------------------------------------
 * Initialization, must be called once on start up.
 *
 */
void spi_init(void);

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
 *
 * Returns: A spi_slave reference that can be used in subsequent SPI
 * calls, or NULL if one or more of the parameters are not supported.
 */
struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs);

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

#endif	/* _SPI_GENERIC_H_ */
