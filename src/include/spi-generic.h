/*
 * (C) Copyright 2001
 * Gerald Van Baren, Custom IDEAS, vanbaren@cideas.com.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _SPI_GENERIC_H_
#define _SPI_GENERIC_H_

#include <stdint.h>

/* Controller-specific definitions: */

/* SPI mode flags */
#define	SPI_CPHA	0x01			/* clock phase */
#define	SPI_CPOL	0x02			/* clock polarity */
#define	SPI_MODE_0	(0|0)			/* (original MicroWire) */
#define	SPI_MODE_1	(0|SPI_CPHA)
#define	SPI_MODE_2	(SPI_CPOL|0)
#define	SPI_MODE_3	(SPI_CPOL|SPI_CPHA)
#define	SPI_CS_HIGH	0x04			/* CS active high */
#define	SPI_LSB_FIRST	0x08			/* per-word bits-on-wire */
#define	SPI_3WIRE	0x10			/* SI/SO signals shared */
#define	SPI_LOOP	0x20			/* loopback mode */

/* SPI transfer flags */
#define SPI_XFER_BEGIN	0x01			/* Assert CS before transfer */
#define SPI_XFER_END	0x02			/* Deassert CS after transfer */

/* SPI opcodes */
#define SPI_OPCODE_WREN 0x06
#define SPI_OPCODE_FAST_READ 0x0b

#define SPI_READ_FLAG	0x01
#define SPI_WRITE_FLAG	0x02

/*-----------------------------------------------------------------------
 * Representation of a SPI slave, i.e. what we're communicating with.
 *
 * Drivers are expected to extend this with controller-specific data.
 *
 *   bus:	ID of the bus that the slave is attached to.
 *   cs:	ID of the chip select connected to the slave.
 *   rw: 	Read or Write flag
 */
struct spi_slave {
	unsigned int	bus;
	unsigned int	cs;
	unsigned int	rw;
	int force_programmer_specific;
	struct spi_flash * (*programmer_specific_probe) (struct spi_slave *spi);
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
 *   max_hz:  Maximum SCK rate in Hz.
 *   mode:    Clock polarity, clock phase and other parameters.
 *
 * Returns: A spi_slave reference that can be used in subsequent SPI
 * calls, or NULL if one or more of the parameters are not supported.
 */
struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int mode);

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
int spi_claim_bus(struct spi_slave *slave);

/*-----------------------------------------------------------------------
 * Release the SPI bus
 *
 * This must be called once for every call to spi_claim_bus() after
 * all transfers have finished. It may disable any SPI hardware as
 * appropriate.
 *
 *   slave:	The SPI slave
 */
void spi_release_bus(struct spi_slave *slave);

/*-----------------------------------------------------------------------
 * SPI transfer
 *
 * This writes "bitlen" bits out the SPI MOSI port and simultaneously clocks
 * "bitlen" bits in the SPI MISO port.  That's just the way SPI works.
 *
 * The source of the outgoing bits is the "dout" parameter and the
 * destination of the input bits is the "din" parameter.  Note that "dout"
 * and "din" can point to the same memory location, in which case the
 * input data overwrites the output data (since both are buffered by
 * temporary variables, this is OK).
 *
 * spi_xfer() interface:
 *   slave:	The SPI slave which will be sending/receiving the data.
 *   dout:	Pointer to a string of bits to send out.  The bits are
 *		held in a byte array and are sent MSB first.
 *   bitsout:	How many bits to write.
 *   din:	Pointer to a string of bits that will be filled in.
 *   bitsin:	How many bits to read.
 *
 *   Returns: 0 on success, not 0 on failure
 */
int  spi_xfer(struct spi_slave *slave, const void *dout, unsigned int bitsout,
		void *din, unsigned int bitsin);

/*-----------------------------------------------------------------------
 * Determine if a SPI chipselect is valid.
 * This function is provided by the board if the low-level SPI driver
 * needs it to determine if a given chipselect is actually valid.
 *
 * Returns: 1 if bus:cs identifies a valid chip on this board, 0
 * otherwise.
 */
int  spi_cs_is_valid(unsigned int bus, unsigned int cs);

/*-----------------------------------------------------------------------
 * Activate a SPI chipselect.
 * This function is provided by the board code when using a driver
 * that can't control its chipselects automatically (e.g.
 * common/soft_spi.c). When called, it should activate the chip select
 * to the device identified by "slave".
 */
void spi_cs_activate(struct spi_slave *slave);

/*-----------------------------------------------------------------------
 * Deactivate a SPI chipselect.
 * This function is provided by the board code when using a driver
 * that can't control its chipselects automatically (e.g.
 * common/soft_spi.c). When called, it should deactivate the chip
 * select to the device identified by "slave".
 */
void spi_cs_deactivate(struct spi_slave *slave);

/*-----------------------------------------------------------------------
 * Set transfer speed.
 * This sets a new speed to be applied for next spi_xfer().
 *   slave:	The SPI slave
 *   hz:	The transfer speed
 */
void spi_set_speed(struct spi_slave *slave, uint32_t hz);

/*-----------------------------------------------------------------------
 * Write 8 bits, then read 8 bits.
 *   slave:	The SPI slave we're communicating with
 *   byte:	Byte to be written
 *
 * Returns: The value that was read, or a negative value on error.
 *
 * TODO: This function probably shouldn't be inlined.
 */
static inline int spi_w8r8(struct spi_slave *slave, unsigned char byte)
{
	unsigned char dout[2];
	unsigned char din[2];
	int ret;

	dout[0] = byte;
	dout[1] = 0;

	ret = spi_xfer(slave, dout, 16, din, 16);
	return ret < 0 ? ret : din[1];
}

#endif	/* _SPI_GENERIC_H_ */
